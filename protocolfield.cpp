#include "protocolfield.h"
#include "protocolparser.h"
#include "shuntingyard.h"
#include <QString>
#include <QDomElement>
#include <math.h>

TypeData::TypeData(ProtocolSupport sup) :
    isStruct(false),
    isSigned(false),
    isBitfield(false),
    isFloat(false),
    isEnum(false),
    isString(false),
    isFixedString(false),
    isNull(false),
    bits(8),
    support(sup)
{
}


TypeData::TypeData(const TypeData& that) :
    isStruct(that.isStruct),
    isSigned(that.isSigned),
    isBitfield(that.isBitfield),
    isFloat(that.isFloat),
    isEnum(that.isEnum),
    isString(that.isString),
    isFixedString(that.isFixedString),
    isNull(that.isNull),
    bits(that.bits),
    support(that.support)
{
}


/*!
 * Reset all members to defaults except the protocol support
 */
void TypeData::clear(void)
{
    isStruct = false;
    isSigned = false;
    isBitfield = false;
    isFloat = false;
    isEnum = false;
    isString = false;
    isFixedString = false;
    isNull = false;
    bits = 8;
}


/*!
 * Pull a positive integer value from a string
 * \param string is the source string, which can contain a decimal or hexadecimal (0x) value
 * \param ok receives false if there was a problem with the decode
 * \return the positive integer value or 0 if there was a problem
 */
int TypeData::extractPositiveInt(const QString& string, bool* ok)
{
    QString number = string;

    if(number.contains("0x", Qt::CaseInsensitive))
    {
        QRegExp rx("[^0123456789AaBbCcDdEeFf]");
        number.replace(rx, "");
        return number.toInt(ok, 16);
    }
    else
    {
        QRegExp rx("[^0123456789]");
        number.replace(rx, "");
        return number.toInt(ok, 10);
    }

}


/*!
 * Pull a double precision value from a string
 * \param string is the source string
 * \param ok receives false if there was a problem with the decode
 * \return the double precision value or 0 if there was a problem
 */
double TypeData::extractDouble(const QString& string, bool* ok)
{
    QString number = string;

    if(number.contains("0x", Qt::CaseInsensitive) && !number.contains("."))
    {
        QRegExp rx("[^0123456789AaBbCcDdEeFf]");
        number.replace(rx, "");
        return (double)number.toInt(ok, 16);
    }
    else
    {
        QRegExp rx("[^0123456789-\\.]");
        number.replace(rx, "");
        return number.toDouble(ok);
    }
}


QString TypeData::toTypeString(QString enumName, QString structName) const
{
    QString typeName;

    if(isString)
        typeName = "char";
    else if(isBitfield)
    {
        if((bits > 32) && (support.longbitfield))
            typeName = "uint64_t";
        else
            typeName = "unsigned";
    }
    else if(isEnum)
    {
        typeName = enumName;
    }
    else if(isStruct)
    {
        typeName = structName;

        // Make user it ends with "_t";
        if(!typeName.contains("_t"))
            typeName += "_t";
    }
    else
    {
        // create the type name
        if(isFloat)
        {
            if(bits > 32)
                typeName = "double";
            else
                typeName = "float";

        }// if floating point type
        else
        {
            if(isSigned)
                typeName = "int";
            else
                typeName = "uint";

            // Add the bits, we use only valid native type widths
            if(bits > 32)
                typeName += "64_t";
            else if(bits > 16)
                typeName += "32_t";
            else if(bits > 8)
                typeName += "16_t";
            else
                typeName += "8_t";

        }// else if integer type
    }

    return typeName;

}// toTypeString


/*!
 * Construct a blank protocol field
 * \param parse points to the global protocol parser that owns everything
 * \param parent is the hierarchical name of the parent object
 * \param protocolName is the name of the protocol
 * \param supported indicates what the protocol can support
 */
ProtocolField::ProtocolField(ProtocolParser* parse, QString parent, const QString& protocolName, ProtocolSupport supported):
    Encodable(parse, parent, protocolName, supported),
    encodedMin(0),
    encodedMax(0),
    scaler(1),
    checkConstant(false),
    inMemoryType(supported),
    encodedType(supported),
    lastBitfield(false),
    startingBitCount(0)
{

}


/*!
 * Reset all data to defaults
 */
void ProtocolField::clear(void)
{
    Encodable::clear();

    enumName.clear();
    encodedMin = encodedMax = 0;
    scaler = 1;
    defaultValue.clear();
    constantValue.clear();
    checkConstant = false;
    encodedType = inMemoryType = TypeData(support);
    scalerString.clear();
    minString.clear();
    maxString.clear();
    lastBitfield = false;
    startingBitCount = 0;
    extraInfoNames.clear();
    extraInfoValues.clear();

}// ProtocolField::clear


/*!
 * Extract the type information from the type string, for in memory types
 * \param data holds the extracted type
 * \param type is the type string
 * \param name is the name of this field, used for warnings
 * \param inMemory is true if this is an in-memory type string, else encoded
 */
void ProtocolField::extractType(TypeData& data, const QString& typeString, const QString& name, bool inMemory)
{
    QString type(typeString);

    data.clear();

    if(type.startsWith("n", Qt::CaseInsensitive))
        data.isNull = true;
    else if(type.startsWith("stru", Qt::CaseInsensitive))
    {
        if(inMemory)
            data.isStruct = true;
        else
            return;
    }
    else if(type.startsWith("string", Qt::CaseInsensitive))
    {
        data.isString = true;
        data.isFixedString = false;

        data.bits = 8;
    }
    else if(type.startsWith("fixedstring", Qt::CaseInsensitive))
    {
        data.isString = true;
        data.isFixedString = true;

        data.bits = 8;
    }
    else if(type.startsWith("b", Qt::CaseInsensitive))
    {
        // Get the number of bits, between 1 and 32 inclusive
        data.bits = data.extractPositiveInt(type);

        if(support.bitfield == false)
        {
            emitWarning("bitfield support is disabled in this protocol");

            // if bits is 1, then it becomes 8. If it is 8 then it
            // becomes 8, if its 9 it becomes 16, etc.
            data.bits = 8*((data.bits + 7)/8);
        }
        else
        {
            // Bitfields cannot be floats and cannot be signed
            data.isBitfield = true;

            // Bitfields must have at least one bit, and less than 33 bits
            if(data.bits < 1)
            {
                data.bits = 1;
                emitWarning("bitfields must have a bit width of at least one");
            }
            else if((data.bits > 32) && (support.longbitfield == false))
            {
                emitWarning("bitfields must have a bit width of 32 or less");
                data.bits = 32;
            }
            else if(data.bits > 64)
            {
                emitWarning("bitfields must have a bit width of 64 or less");
                data.bits = 64;
            }
        }
    }
    else if(type.startsWith("e", Qt::CaseInsensitive))
    {
        // enumeration types are only for in-memory, never encoded
        if(inMemory)
        {
            data.isEnum = true;
        }
        else
        {
            data.isEnum = false;
        }

        data.bits = 8;
    }
    else
    {
        data.bits = data.extractPositiveInt(type);

        if(type.startsWith("u", Qt::CaseInsensitive))
        {
            data.isSigned = false;
        }
        else
        {
            data.isSigned = true;

            if(type.startsWith("f", Qt::CaseInsensitive))
            {
                data.isFloat = true;

                // "float" is not a warning
                if(data.bits == 0)
                    data.bits = 32;
            }
            else if(type.startsWith("d", Qt::CaseInsensitive))
            {
                data.isFloat = true;

                // "double" is not a warning
                if(data.bits == 0)
                    data.bits = 64;
            }
            else if(!type.startsWith("s", Qt::CaseInsensitive) && !type.startsWith("i", Qt::CaseInsensitive))
            {
                emitWarning("in memory type name not understood, signed integer assumed");
            }
        }

        if(data.isFloat)
        {
            if(inMemory)
            {
                if((data.bits != 32) && (data.bits != 64))
                {
                    emitWarning("in memory float types must be 32 or 64 bits");

                    if(data.bits < 32)
                        data.bits = 32;
                    else
                        data.bits = 64;
                }
            }
            else if((data.bits != 16) && (data.bits != 24) && (data.bits != 32) && (data.bits != 64))
            {
                emitWarning("encoded float types must be 16, 24, 32, or 64 bits");

                if(data.bits < 16)
                    data.bits = 16;
                else if(data.bits < 24)
                    data.bits = 24;
                else if(data.bits < 32)
                    data.bits = 32;
                else
                    data.bits = 64;

                if((data.bits < 32) && (support.specialFloat == false))
                {
                    emitWarning("non-standard float bit widths are disabled in this protocol");
                    data.bits = 32;
                }
            }

            if((data.bits > 32) && (support.float64 == false))
            {
                emitWarning("64 bit float support is disabled in this protocol");
                data.bits = 32;
            }

        }// if float
        else
        {
            if(inMemory)
            {
                if((data.bits != 8) && (data.bits != 16) && (data.bits != 32) && (data.bits != 64))
                {
                    emitWarning("in memory integer types must be 8, 16, 32, or 64 bits");

                    if(data.bits > 32)
                        data.bits = 64;
                    else if(data.bits > 16)
                        data.bits = 32;
                    else if(data.bits > 8)
                        data.bits = 16;
                    else
                        data.bits = 8;
                }
            }
            else if(((data.bits % 8) != 0) || (data.bits > 64))
            {
                emitWarning("encoded integer types must be 8, 16, 24, 32, 40, 48, 56, or 64 bits");

                if(data.bits > 56)
                    data.bits = 64;
                else if(data.bits > 48)
                    data.bits = 56;
                else if(data.bits > 40)
                    data.bits = 48;
                else if(data.bits > 32)
                    data.bits = 40;
                else if(data.bits > 24)
                    data.bits = 32;
                else if(data.bits > 16)
                    data.bits = 24;
                else if(data.bits > 8)
                    data.bits = 16;
                else
                    data.bits = 8;
            }

            if((data.bits > 32) && (support.int64 == false))
            {
                emitWarning("Integers greater than 32 bits are disabled in this protocol");
                data.bits = 32;
            }

        }// if integer

    }// else if float or integer

}// ProtocolField::extractType


/*!
 * Parse the DOM to determine the details of this ProtocolField
 */
void ProtocolField::parse(void)
{
    QString memoryTypeString;
    QString encodedTypeString;
    QString structName;

    clear();

    QDomNamedNodeMap map = e.attributes();

    // We use name as part of our debug outputs, so its good to have it first.
    name = ProtocolParser::getAttribute("name", map);

    for(int i = 0; i < map.count(); i++)
    {
        QDomAttr attr = map.item(i).toAttr();
        if(attr.isNull())
            continue;

        QString attrname = attr.name();

        if(attrname.compare("name", Qt::CaseInsensitive) == 0)
            name = attr.value().trimmed();
        else if(attrname.compare("inMemoryType", Qt::CaseInsensitive) == 0)
            memoryTypeString = attr.value().trimmed();
        else if(attrname.compare("encodedType", Qt::CaseInsensitive) == 0)
            encodedTypeString = attr.value().trimmed();
        else if(attrname.compare("struct", Qt::CaseInsensitive) == 0)
            structName = attr.value().trimmed();
        else if(attrname.compare("max", Qt::CaseInsensitive) == 0)
            maxString = attr.value().trimmed();
        else if(attrname.compare("min", Qt::CaseInsensitive) == 0)
            minString = attr.value().trimmed();
        else if(attrname.compare("scaler", Qt::CaseInsensitive) == 0)
            scalerString = attr.value().trimmed();
        else if(attrname.compare("array", Qt::CaseInsensitive) == 0)
            array = attr.value().trimmed();
        else if(attrname.compare("variableArray", Qt::CaseInsensitive) == 0)
            variableArray = attr.value().trimmed();
        else if(attrname.compare("array2d", Qt::CaseInsensitive) == 0)
            array2d = attr.value().trimmed();
        else if(attrname.compare("variable2dArray", Qt::CaseInsensitive) == 0)
            variable2dArray = attr.value().trimmed();
        else if(attrname.compare("dependsOn", Qt::CaseInsensitive) == 0)
            dependsOn = attr.value().trimmed();
        else if(attrname.compare("enum", Qt::CaseInsensitive) == 0)
            enumName = attr.value().trimmed();
        else if(attrname.compare("default", Qt::CaseInsensitive) == 0)
            defaultValue = attr.value().trimmed();
        else if(attrname.compare("constant", Qt::CaseInsensitive) == 0)
            constantValue = attr.value().trimmed();
        else if(attrname.compare("checkConstant", Qt::CaseInsensitive) == 0)
            checkConstant = ProtocolParser::isFieldSet(attr.value().trimmed());
        else if(attrname.compare("comment", Qt::CaseInsensitive) == 0)
            comment = ProtocolParser::reflowComment(attr.value().trimmed());
        else if(attrname.compare("Units", Qt::CaseInsensitive) == 0)
        {
            extraInfoNames.append("Units");
            extraInfoValues.append(attr.value());
        }
        else if(attrname.compare("Range", Qt::CaseInsensitive) == 0)
        {
            extraInfoNames.append("Range");
            extraInfoValues.append(attr.value());
        }
        else if(attrname.compare("Notes", Qt::CaseInsensitive) == 0)
        {
            extraInfoNames.append("Notes");
            extraInfoValues.append(attr.value());
        }
        else if(support.disableunrecognized == false)
        {
            emitWarning("Unrecognized attribute of Data: " + attrname);
        }

    }// for all attributes


    if(name.isEmpty() && (memoryTypeString != "null"))
    {
        emitWarning("Data tag without a name: " + e.text());
    }

    // maybe its an enum or a external struct?
    if(memoryTypeString.isEmpty())
    {
        // Maybe its an enum?
        if(!e.attribute("enum").isEmpty())
            memoryTypeString = "enum";
        else if(!e.attribute("struct").isEmpty())
            memoryTypeString = "struct";
        else
        {
            memoryTypeString = "null";
            emitWarning("failed to find inMemoryType attribute, \"null\" assumed.");
        }
    }

    // Extract the in memory type
    extractType(inMemoryType, memoryTypeString, name, true);

    // The encoded type string, this can be empty which implies encoded is same as memory
    if(encodedTypeString.isEmpty())
    {
        encodedType = inMemoryType;

        // Encoded types are never enums
        if(encodedType.isEnum)
            encodedType.isEnum = false;
    }
    else
        extractType(encodedType, encodedTypeString, name, false);

    if(inMemoryType.isNull)
    {
        // Null types are not in memory, therefore cannot have defaults or variable arrays
        variableArray.clear();
        variable2dArray.clear();
        defaultValue.clear();

        // A special case, where we use the encoded type data in place of the
        // in memory type. This handles cases where (for example) we want to
        // encode a constant bitfield, or a string, but have no data in memory
        if(!encodedType.isNull)
        {
            inMemoryType = encodedType;
            inMemoryType.isNull = true;
        }
        else
        {
            emitWarning("both in-memory and encoded types are \"null\", nothing to do.");
            return;
        }
    }

    if(inMemoryType.isEnum)
    {
        if(enumName.isEmpty())
        {
            emitWarning("enumeration name is missing, type changed to unsigned");
            inMemoryType.isEnum = encodedType.isEnum = false;
        }
        else
        {
            int minbits = 8;

            // Figure out the minimum number of bits for the enumeration
            const EnumCreator* creator = parser->lookUpEnumeration(enumName);
            if(creator != 0)
                minbits = creator->getMinBitWidth();

            if(encodedTypeString.isEmpty())
            {
                // Make it a multiple of 8 bits. The only way to have something
                // different is to encode as a bitfield, which means the
                // encoded string won't be empty
                if(minbits % 8)
                    minbits = ((minbits/8)+1)*8;

                encodedType.bits = minbits;

            }// if we don't have encoded length data
            else
            {
                // make sure the encoded length data is large enough
                if(encodedType.bits < minbits)
                {
                    emitWarning("enumeration needs at least " + QString::number(minbits) + " bits. Encoded bit length changed.");
                    if(!encodedType.isBitfield)
                    {
                        // Make it a multiple of 8 bits
                        if(minbits % 8)
                            minbits = ((minbits/8)+1)*8;
                    }

                    encodedType.bits = minbits;

                }// if not enough encoded bits

            }// else we do have encoded length data

        }// if we have the enumeration name

    }// if in memory data is enumeration

    if(inMemoryType.isStruct)
    {
        if(structName.isEmpty())
        {
            emitWarning("struct name is missing, struct name \"unknown\" used, probable compile failure");
            structName = "unknown";
        }

        if(!constantValue.isEmpty() || checkConstant)
        {
            constantValue.clear();
            checkConstant = false;
            emitWarning("structure cannot be a constant");
        }
    }

    if(inMemoryType.isBitfield)
    {
        if(!encodedType.isNull)
        {
            if(!encodedTypeString.isEmpty() && !encodedType.isBitfield)
                emitWarning("encoded type ignored because in memory type is bitfield");

            // make the encoded type follow the in memory type for bit fields
            encodedType.isBitfield = true;
            encodedType.bits = inMemoryType.bits;
        }
    }

    // It is possible for the in memory type to not be a bit field, but the
    // encoding could be. The most common case for this would be an in-memory
    // enumeration in which the maximum enumeration fits in fewer than 8 bits
    if(encodedType.isBitfield)
    {
        if(!dependsOn.isEmpty())
        {
            emitWarning("bitfields cannot use dependsOn");
            dependsOn.clear();
        }

        if(!array.isEmpty())
        {
            emitWarning("bitfields encodings cannot use arrays");
            array.clear();
            variableArray.clear();
            array2d.clear();
            variable2dArray.clear();
        }

    }

    // if either type says string, than they both are string
    if(inMemoryType.isString != encodedType.isString)
    {
        if(!inMemoryType.isNull && !encodedType.isNull)
            emitWarning("String type requires that inMemory and encoded types both be strings");

        inMemoryType.isString = encodedType.isString = true;
        inMemoryType.bits = encodedType.bits = 8;
    }

    // if either type says fixed string, than they both are fixed string
    if(inMemoryType.isFixedString != encodedType.isFixedString)
    {
        inMemoryType.isString = encodedType.isString = true;
        inMemoryType.isFixedString = encodedType.isFixedString = true;
        inMemoryType.bits = encodedType.bits = 8;
    }

    if(array.isEmpty() && !variableArray.isEmpty())
    {
        emitWarning("Must specify array length to specify variable array length");
        variableArray.clear();
    }

    if(array.isEmpty() && !array2d.isEmpty())
    {
        emitWarning("Must specify array length to specify second dimension array length");
        array2d.clear();
    }

    if(array2d.isEmpty() && !variable2dArray.isEmpty())
    {
        emitWarning("Must specify array 2d length to specify variable 2d array length");
        variable2dArray.clear();
    }

    if(!dependsOn.isEmpty() && !variableArray.isEmpty())
    {
        emitWarning("variable length arrays cannot also use dependsOn");
        dependsOn.clear();
    }

    if(!dependsOn.isEmpty() && !variable2dArray.isEmpty())
    {
        emitWarning("variable length 2d arrays cannot also use dependsOn");
        dependsOn.clear();
    }

    if(!scalerString.isEmpty() && !maxString.isEmpty())
    {
        emitWarning("scaler ignored because max is provided");
        scalerString.clear();
    }

    if(!maxString.isEmpty() || !minString.isEmpty() || !scalerString.isEmpty())
    {
        if(inMemoryType.isStruct || inMemoryType.isString || encodedType.isNull)
        {
            emitWarning("min, max, and scaler do not apply to this type data");
            maxString.clear();
            minString.clear();
            scalerString.clear();
        }

    }


    if(!maxString.isEmpty() || !minString.isEmpty())
    {
        if(encodedType.isFloat)
        {
            emitWarning("min, max, are ignored because encoded type is float");
            maxString.clear();
            minString.clear();
        }

    }

    if(constantValue.isEmpty() && checkConstant)
    {
        emitWarning("\"checkConstant\" cannot be applied unless the field is constant");
        checkConstant = false;
    }

    if(inMemoryType.isString)
    {
        // Strings have to be arrays, default to 64 characters
        if(array.isEmpty())
        {
            emitWarning("string length not provided, assuming 64");
            array = "64";
        }

        // Strings are always variable length, through null termination
        if(!variableArray.isEmpty())
        {
            emitWarning("strings cannot use variableAray attribute, they are always variable length through null termination (unless fixedstring)");
            variableArray.clear();
        }

        if(!array2d.isEmpty())
        {
            emitWarning("2d arrays not allowed for strings");
            array2d.clear();
            variable2dArray.clear();
        }

        if(!dependsOn.isEmpty())
        {
            emitWarning("strings cannot use dependsOn");
            dependsOn.clear();
        }

    }// if string
    else if(!array.isEmpty())
    {
        if(checkConstant)
        {
            emitWarning("\"checkConstant\" cannot be applied to arrays (except strings) ");
            checkConstant = false;
        }

    }// if not string

    if(encodedType.isNull)
    {
        if(!constantValue.isEmpty())
        {
            emitWarning("constant value does not make sense for types that are not encoded (null)");
            constantValue.clear();
            checkConstant = false;
        }

        if(!variableArray.isEmpty() || !variable2dArray.isEmpty())
        {
            emitWarning("variable length arrays do not make sense for types that are not encoded (null)");
            variableArray.clear();
            variable2dArray.clear();
        }

        if(!dependsOn.isEmpty())
        {
            emitWarning("dependsOn does not make sense for types that are not encoded (null)");
            dependsOn.clear();
        }
    }

    bool ok;

    if(!minString.isEmpty())
    {
        if(encodedType.isSigned)
        {
            emitWarning("min value ignored because encoded type is signed");
            minString.clear();
        }
        else
        {
            encodedMin = ShuntingYard::computeInfix(minString, &ok);

            if(!ok)
            {
                emitWarning("min is not a number, 0.0 assumed");
                minString = "0";
            }
        }
    }

    if(!maxString.isEmpty())
    {
        encodedMax = ShuntingYard::computeInfix(maxString, &ok);
        if(!ok)
        {
            emitWarning("max is not a number, 1.0 assumed");
            encodedMax = 1.0;
            maxString = "1";
        }

        if(encodedType.isSigned)
        {
            scaler = (powf(2.0, encodedType.bits-1) - 1.0)/encodedMax;

            scalerString = QString().setNum(pow2(encodedType.bits-1)-1)+ "/(" + maxString + ")";

            // This is not exactly true, there is one more bit that could be used,
            // but this makes conciser commenting, and is clearer to the user
            encodedMin = -encodedMax;
            minString = "-" + maxString;
        }
        else
        {
            scaler = (powf(2.0, encodedType.bits) - 1.0)/(encodedMax - encodedMin);

            if(encodedMin == 0.0)
            {
                minString = "0";
                scalerString = QString().setNum(pow2(encodedType.bits)-1)+ "/(" + maxString + ")";
            }
            else
            {
                // If the user gives us something like 145 for max and -5 for min, I would rather just put 150 in the documentation
                QString denominator = "(" + maxString + " - " + minString + ")";

                // Documentation is only improved if maxString and minString are simple numbers, not formulas
                if(ShuntingYard::isNumber(maxString) && ShuntingYard::isNumber(minString))
                {
                    bool ok;
                    double number = ShuntingYard::computeInfix(denominator, &ok);
                    if(ok)
                        denominator = getNumberString(number);
                }

                scalerString = QString().setNum(pow2(encodedType.bits)-1)+ "/" + denominator;
            }
        }
    }
    else if(!scalerString.isEmpty())
    {
        scaler = ShuntingYard::computeInfix(scalerString, &ok);

        if(!ok)
        {
            emitWarning("scaler is not a number, 1.0 assumed");
            scaler = 1.0;
            scalerString = "1.0";
        }
        else if(scaler <= 0.0)
        {
            emitWarning("scaler must be greater than zero, 1.0 used");
            scaler = 1.0;
            scalerString = "1.0";
        }

        if(encodedType.isFloat)
        {
            // Floating point scaling does not have min and max.
            encodedMax = encodedMin = 0;
            minString = maxString = "";
        }
        else if(encodedType.isSigned)
        {
            encodedMax = (powf(2.0, encodedType.bits-1) - 1.0)/scaler;
            maxString = QString().setNum(pow2(encodedType.bits-1)-1)+ "/(" + scalerString + ")";

            // This is not exactly true, there is one more bit that could be used,
            // but this makes conciser commenting, and is clearer to the user
            encodedMin = -encodedMax;
            minString = "-" + maxString;
        }
        else
        {
            encodedMax = encodedMin + (powf(2.0, encodedType.bits) - 1.0)/scaler;

            // Make sure minString isn't empty
            if(encodedMin == 0)
            {
                minString = "0";
                maxString = QString().setNum(pow2(encodedType.bits)-1)+ "/(" + scalerString + ")";
            }
            else
                maxString = minString + " + " + QString().setNum(pow2(encodedType.bits)-1)+ "/(" + scalerString + ")";
        }

    }// if scaler string

    // Change so html will render the pi symbol
    maxString.replace("pi", "&pi;");
    minString.replace("pi", "&pi;");
    scalerString.replace("pi", "&pi;");

    // Change to get rid of * multiply symbol, which plays havoc with markdown
    maxString.replace("*", "&times;");
    minString.replace("*", "&times;");
    scalerString.replace("*", "&times;");

    // Max must be larger than minimum
    if(encodedMin > encodedMax)
    {
        encodedMin = encodedMax = 0.0;
        minString.clear();
        maxString.clear();
        scaler = 1.0;
        emitWarning("max is not more than min, encoding not scaled");
    }

    // Just the type data
    typeName = inMemoryType.toTypeString(enumName, support.prefix + structName);

    if(!constantValue.isEmpty())
    {
        if(inMemoryType.isStruct)
        {
            emitWarning("structure cannot have a constant value");
            constantValue.clear();
        }
        else if(!defaultValue.isEmpty())
        {
            emitWarning("fields with default values cannot also be constant");
            constantValue.clear();
        }
    }

    // Make sure no keyword conflicts
    checkAgainstKeywords();

    // Compute the data length
    computeEncodedLength();

}// ProtocolField::parse


/*!
 * Compute the encoded length of this field
 */
void ProtocolField::computeEncodedLength(void)
{
    encodedLength.clear();

    if(encodedType.isNull)
        return;

    if(encodedType.isBitfield)
    {
        int length = 0;

        // As a bitfield our length in bytes is given by the number of 8 bit boundaries we cross
        int bitcount = startingBitCount;
        while(bitcount < getEndingBitCount())
        {
            bitcount++;
            if((bitcount % 8) == 0)
                length++;
        }

        // If we are the last bitfield, and if we have any bits left, then add a byte
        if(lastBitfield && ((bitcount % 8) != 0))
            length++;

        encodedLength.addToLength(QString().setNum(length));
    }
    else if(inMemoryType.isString)
        encodedLength.addToLength(array, !inMemoryType.isFixedString, false, !dependsOn.isEmpty(), !defaultValue.isEmpty());
    else if(inMemoryType.isStruct)
    {
        encodedLength.clear();

        const ProtocolStructure* struc = parser->lookUpStructure(typeName);

        // Account for array, variable array, and depends on
        if(struc != NULL)
            encodedLength.addToLength(struc->encodedLength, array, !(variableArray.isEmpty() || variable2dArray.isEmpty()), !dependsOn.isEmpty(), array2d);
        else
        {
            if(is2dArray())
                encodedLength.addToLength("getMinLengthOf" + typeName + "()*" + array + "*" + array2d, false, !(variableArray.isEmpty() || variable2dArray.isEmpty()), !dependsOn.isEmpty(), !defaultValue.isEmpty());
            else if(isArray())
                encodedLength.addToLength("getMinLengthOf" + typeName + "()*" + array, false, !variableArray.isEmpty(), !dependsOn.isEmpty(), !defaultValue.isEmpty());
            else
                encodedLength.addToLength("getMinLengthOf" + typeName + "()"         , false, false,                    !dependsOn.isEmpty(), !defaultValue.isEmpty());
        }
    }
    else
    {
        QString lengthString;

        lengthString.setNum(encodedType.bits / 8);

        // Remember that we could be encoding an array
        if(isArray())
            lengthString += "*" + array;

        if(is2dArray())
            lengthString += "*" + array2d;

        encodedLength.addToLength(lengthString, false, !variableArray.isEmpty(), !dependsOn.isEmpty(), !defaultValue.isEmpty());

    }

}// ProtocolField::computeEncodedLength


//! Check names against the list of C keywords
void ProtocolField::checkAgainstKeywords(void)
{
    Encodable::checkAgainstKeywords();

    if(keywords.contains(enumName))
    {
        emitWarning("enum name matches C keyword, changed to _name");
        enumName = "_" + enumName;
    }

    if(keywords.contains(maxString))
    {
        emitWarning("max value matches C keyword, changed to _max");
        maxString = "_" + maxString;
    }

    if(keywords.contains(minString))
    {
        emitWarning("min value matches C keyword, changed to _min");
        minString = "_" + minString;
    }

    if(keywords.contains(scalerString))
    {
        emitWarning("scaler value matches C keyword, changed to _scaler");
        scalerString = "_" + scalerString;
    }

    if(keywords.contains(defaultValue))
    {
        emitWarning("default value matches C keyword, changed to _default");
        defaultValue = "_" + defaultValue;
    }

    if(keywords.contains(constantValue))
    {
        emitWarning("constant value matches C keyword, changed to _constant");
        constantValue = "_" + constantValue;
    }

}// ProtocolField::checkAgainstKeywords


/*!
 * Get the declaration for this field as a member of a structure
 * \return the declaration string
 */
QString ProtocolField::getDeclaration(void) const
{
    QString output;

    if(isNotInMemory())
        return output;

    output = "    " + typeName + " " + name;

    if(inMemoryType.isBitfield)
        output += " : " + QString().setNum(inMemoryType.bits);
    else if(is2dArray())
        output += "[" + array + "][" + array2d + "]";
    else if(isArray())
        output += "[" + array + "]";

    output += ";";

    if(comment.isEmpty())
    {
        if(!constantValue.isEmpty())
            output += " //!< Field is encoded constant.";
    }
    else
    {
        output += " //!< " + comment;
        if(!constantValue.isEmpty())
            output += ". Field is encoded constant.";
    }

    output += "\n";

    return output;

}// ProtocolField::getDeclaration


/*!
 * Return the inclue directive needed for this encodable. Mostly this is empty,
 * but for external structures we need to bring in the include file
 */
QString ProtocolField::getIncludeDirective(void)
{
    if(inMemoryType.isStruct)
    {
        QString output = parser->lookUpIncludeName(typeName);

        if(output.isEmpty())
        {
            if(!isNotEncoded())
            {
                // In this case, we guess at the include name
                output = typeName;
                output.remove("_t");
                output += ".h";
                emitWarning("unknown include for " + typeName + "; guess supplied");
            }
            else
            {
                // If there is no encoding for this field, then we assume its
                // some externally supplied struct definition, and its up to the
                // user to specify the correct include file, since we don't need
                // the encoding functions anyway.
                return QString();
            }
        }

        return output;
    }
    else
        return QString();
}


/*!
 * Return the signature of this field in an encode function signature
 * \return The encode signature of this field
 */
QString ProtocolField::getEncodeSignature(void) const
{
    if(isNotEncoded() || isNotInMemory() || isConstant())
        return "";
    else if(is2dArray())
        return ", const " + typeName + " " + name + "[" + array + "][" + array2d + "]";
    else if(isArray())
        return ", const " + typeName + " " + name + "[" + array + "]";
    else if(!inMemoryType.isStruct)
        return ", " + typeName + " " + name;
    else
        return ", const " + typeName + "* " + name;
}


/*!
 * Get details needed to produce documentation for this encodable.
 * \param outline gives the paragraph numbers for all the fields up to this one.
 * \param startByte is the starting byte location of this encodable, which will be updated for the following encodable.
 * \param bytes is appended for the byte range of this encodable.
 * \param names is appended for the name of this encodable.
 * \param encodings is appended for the encoding of this encodable.
 * \param repeats is appended for the array information of this encodable.
 * \param comments is appended for the description of this encodable.
 */
void ProtocolField::getDocumentationDetails(QList<int>& outline, QString& startByte, QStringList& bytes, QStringList& names, QStringList& encodings, QStringList& repeats, QStringList& comments) const
{
    QString description;
    QString maxEncodedLength = encodedLength.maxEncodedLength;

    if(encodedType.isNull)
        return;

    // See if we can replace any enumeration names with values
    parser->replaceEnumerationNameWithValue(maxEncodedLength);

    // The byte after this one
    QString nextStartByte = EncodedLength::collapseLengthString(startByte + "+" + maxEncodedLength);

    // The length data
    if(encodedType.isBitfield)
    {
        QString range;

        // the starting bit count is the full count, not the count in the byte
        int startCount = startingBitCount % 8;

        if(startByte.isEmpty())
            range = "0:" + QString().setNum(7 - startCount);
        else
            range = startByte + ":" + QString().setNum(7 - startCount);

        if(encodedType.bits > 1)
        {
            int endCount = (startCount + encodedType.bits - 1);

            int byteCount = endCount / 8;

            QString endByte = EncodedLength::collapseLengthString(startByte + "+" + QString().setNum(byteCount), true);

            range += "..." + endByte + ":" + QString().setNum(7 - (endCount%8));
        }

        bytes.append(range);
    }
    else
    {
        if(maxEncodedLength.isEmpty() || (maxEncodedLength.compare("1") == 0))
            bytes.append(startByte);
        else
        {
            QString endByte = EncodedLength::subtractOneFromLengthString(nextStartByte);

            // The range of the data
            bytes.append(startByte + "..." + endByte);
        }
    }

    // The name information
    outline.last() += 1;
    QString outlineString;
    outlineString.setNum(outline.at(0));
    for(int i = 1; i < outline.size(); i++)
        outlineString += "." + QString().setNum(outline.at(i));

    if(inMemoryType.isEnum)
    {
        // Link to the enumeration
        outlineString += ")[" + name +"](#" + enumName + ")";
    }
    else
        outlineString += ")" + name;

    names.append(outlineString);

    if(inMemoryType.isStruct)
    {
        // Encoding is blank for structures
        encodings.append(QString());

        // Repeats
        repeats.append(getRepeatsDocumentationDetails());

        // Fourth column is the commenting
        description += comment;

        if(!dependsOn.isEmpty())
        {
            if(!description.endsWith('.'))
                description += ".";

            description += " Only included if " + dependsOn + " is non-zero.";
        }

        if(description.isEmpty())
            comments.append(QString());
        else
            comments.append(description);

        QString subStartByte = startByte;

        // Now go get the substructure stuff
        parser->getStructureSubDocumentationDetails(typeName, outline, subStartByte, bytes, names, encodings, repeats, comments);

    }// if structure
    else
    {
        // The encoding
        if(encodedType.isFixedString)
        {
            encodings.append("Zero terminated string of exactly " + array + " bytes");
            repeats.append(QString());
        }
        else if( encodedType.isString)
        {
            encodings.append("Zero-terminated string up to " + array + " bytes");
            repeats.append(QString());
        }
        else
        {
            if(encodedType.isBitfield)
                encodings.append("B" + QString().setNum(encodedType.bits));
            else if(encodedType.isFloat)
                encodings.append("F" + QString().setNum(encodedType.bits));
            else if(encodedType.isSigned)
                encodings.append("I" + QString().setNum(encodedType.bits));
            else
                encodings.append("U" + QString().setNum(encodedType.bits));

            // Third column is the repeat/array column
            repeats.append(getRepeatsDocumentationDetails());
        }

        // Fourth column is the commenting
        if(inMemoryType.isNull)
        {
            if(!comment.isEmpty())
                description += comment;
            else
            {
                if(encodedType.isBitfield)
                    description += "Reserved bits in the packet.";
                else
                    description += "Reserved bytes in the packet.";
            }
        }
        else
            description += comment;

        if(!description.isEmpty() && !description.endsWith('.'))
            description += ".";

        if(encodedMax > encodedMin)
        {
            if(encodedType.isFloat)
                description += "<br>Scaled by " + scalerString + ".";
            else
                description += "<br>Scaled by " + scalerString + " from " + getDisplayNumberString(encodedMin) + " to " + getDisplayNumberString(encodedMax) + ".";
        }

        if(!constantValue.isEmpty())
            description += "<br>Data are given constant value on encode " + constantValue + ".";

        if(!dependsOn.isEmpty())
            description += "<br>Only included if " + dependsOn + " is non-zero.";

        if(!defaultValue.isEmpty())
            description += "<br>This field is optional. If it is not included then the value is assumed to be " + defaultValue + ".";

        for (int i=0;i<extraInfoNames.count();i++)
        {
            if ((extraInfoValues.count() > i) && (!extraInfoValues.at(i).isEmpty()))
                description += "<br>" + extraInfoNames.at(i) + ": " + extraInfoValues.at(i) + ".";
        }

        // StringList cannot be empty
        if(description.isEmpty())
            comments.append(QString());
        else
            comments.append(description);

    }// else if not structure

    // Update startByte for following encodables
    startByte = nextStartByte;

}// ProtocolField::getDocumentationDetails


/*!
 * Get the next lines(s) of source coded needed to encode this field
 * \param isBigEndian should be true for big endian encoding.
 * \param bitcount points to the running count of bits in a bitfields and should persist between calls
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \return The string to add to the source file that encodes this field.
 */
QString ProtocolField::getEncodeString(bool isBigEndian, int* bitcount, bool isStructureMember) const
{
    QString output;

    if(encodedType.isBitfield)
    {
        output = getEncodeStringForBitfield(bitcount, isStructureMember);

        // Close the bitfield if its the last one
        if(lastBitfield && ((*bitcount) != 0))
            output += getCloseBitfieldString(bitcount);
    }
    else
    {
        if(inMemoryType.isString)
            output += getEncodeStringForString(isStructureMember);
        else if(inMemoryType.isStruct)
            output += getEncodeStringForStructure(isStructureMember);
        else
            output += getEncodeStringForField(isBigEndian, isStructureMember);
    }

    return output;

}


/*!
 * Get the next lines(s) of source coded needed to decode this field
 * \param isBigEndian should be true for big endian encoding.
 * \param bitcount points to the running count of bits in a bitfields and should persist between calls
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \param defaultEnabled should be true to handle defaults
 * \return The string to add to the source file that encodes this field.
 */
QString ProtocolField::getDecodeString(bool isBigEndian, int* bitcount, bool isStructureMember, bool defaultEnabled) const
{
    QString output;

    if(encodedType.isBitfield)
    {
        output += getDecodeStringForBitfield(bitcount, isStructureMember, defaultEnabled);

        // Close the bitfield if its the last one
        if(lastBitfield && ((*bitcount) != 0))
            output += getCloseBitfieldString(bitcount);
    }
    else
    {
        if(inMemoryType.isString)
            output += getDecodeStringForString(isStructureMember);
        else if(inMemoryType.isStruct)
            output += getDecodeStringForStructure(isStructureMember);
        else
            output += getDecodeStringForField(isBigEndian, isStructureMember, defaultEnabled);
    }

    return output;
}


/*!
 * Return the string that sets this encodable to its default value in code
 * \param isStructureMember should be true if this field is accessed through a "user" structure pointer
 * \return the string to add to the source file, including line feed
 */
QString ProtocolField::getSetToDefaultsString(bool isStructureMember) const
{
    QString output;
    QString access;

    if(defaultValue.isEmpty())
        return output;

    // Write out the defaults code
    if(inMemoryType.isString)
    {
        if(isStructureMember)
            access = "user->";

        if(defaultValue.compare("null", Qt::CaseInsensitive) == 0)
            output += "    " + access + name + "[0] = 0;\n";
        else
            output += "    strncpy((char*)" + access + name + ", \"" + defaultValue + "\", " + array + ");\n";
    }
    else if(isArray())
    {
        if(isStructureMember)
            access = "user->";

        if(is2dArray())
        {
            output += "    for(i = 0; i < " + array + "; i++)\n";
            output += "        for(j = 0; j < " + array2d + "; j++)\n";
            output += "            " + access + name + "[i][j] = " + defaultValue + ";\n";
        }
        else
        {
            output += "    for(i = 0; i < " + array + "; i++)\n";
            output += "        " + access + name + "[i] = " + defaultValue + ";\n";
        }
    }
    else
    {
        if(isStructureMember)
            access = "user->";
        else
            access = "*";

        // Direct pointer access
        output += "    " + access + name + " = " + defaultValue + ";\n";
    }

    return output;

}// ProtocolField::getSetToDefaultsString


/*!
 * Get the next lines(s) of source coded needed to encode this bitfield field
 * \param bitcount points to the running count of bits in this string of
 *        bitfields, and will be updated by this fields encoded bit count.
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \return The string to add to the source file that encodes this field.
 */
QString ProtocolField::getEncodeStringForBitfield(int* bitcount, bool isStructureMember) const
{
    QString argument;
    QString output;
    QString constantstring = getConstantString();

    if(encodedType.isNull)
        return output;

    if(!comment.isEmpty())
        output += "    // " + comment + "\n";

    if(constantstring.isEmpty())
    {
        if(isStructureMember)
            argument = "user->" + name;
        else
            argument = name;
    }
    else
        argument = constantstring;

    // check to see if this is a scaled bitfield
    if(encodedMax > encodedMin)
    {
        // Additional commenting to describe the scaling
        output += "    // Range of " + name + " is " + getNumberString(encodedMin) + " to " + getNumberString(encodedMax) +  ".\n";

        if(encodedType.bits > 32)
        {
            if(support.longbitfield)
            {
                if(support.float64)
                    argument = "float64ScaledToLongBitfield((double)" + argument;
                else
                    argument = "float32ScaledToLongBitfield((float)" + argument;
            }
            else
            {
                if(support.float64)
                   argument = "float64ScaledToBitfield((double)" + argument;
                else
                   argument = "float32ScaledToBitfield((float)" + argument;

            }
        }
        else
            argument = "float32ScaledToBitfield((float)" + argument;

        argument += ", " + getNumberString(encodedMin, encodedType.bits);
        argument += ", " + getNumberString(scaler, encodedType.bits);
        argument += ")";
    }
    else
    {
        if((encodedType.bits > 32) && (support.longbitfield))
            argument = "(uint64_t)" + argument;
        else
            argument = "(unsigned int)" + argument;
    }

    output += "    encode";

    if((encodedType.bits > 32) && (support.longbitfield))
        output += "Long";

    output += "Bitfield(" + argument + ", data, &byteindex, &bitcount, " + QString().setNum(encodedType.bits) + ");\n";
    *bitcount += encodedType.bits;

    return output;

}// ProtocolField::getEncodeBitfieldString


/*!
 * Get the next lines(s) of source coded needed to decode this bitfield field
 * \param bitcount points to the running count of bits in this string of
 *        bitfields, and will be updated by this fields encoded bit count.
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \param defaultEnabled should be true to enable defaults for this decode
 * \return The string to add to the source file that decodes this field.
 */
QString ProtocolField::getDecodeStringForBitfield(int* bitcount, bool isStructureMember, bool defaultEnabled) const
{
    QString output;
    QString spacing = "    ";

    if(encodedType.isNull)
        return output;

    // If this field has a default value
    if(defaultEnabled && !defaultValue.isEmpty())
    {
        // Number of bytes. From 1 to 8 bits is one byte, from 9 to 16 bits is two bytes
        QString lengthString = QString::number((encodedType.bits + 7)/8);

        output += spacing + "if(byteindex + " + lengthString + " > numBytes)\n";
        output += spacing + "    return 1;\n";
        output += spacing + "else\n";
        output += spacing + "{\n";
        spacing += "    ";
    }

    if(!comment.isEmpty())
        output += spacing + "// " + comment + "\n";

    QString constantstring = getConstantString();

    // The actual decode code is the same no matter the trimmings around it
    QString decodestring;

    if((encodedType.bits > 32) && (support.longbitfield))
        decodestring = "decodeLongBitfield(data, &byteindex, &bitcount, " + QString().setNum(encodedType.bits) + ")";
    else
        decodestring = "decodeBitfield(data, &byteindex, &bitcount, " + QString().setNum(encodedType.bits) + ")";

    // Check for scaled bitfield, which adds to the decode string
    if(encodedMax > encodedMin)
    {
        // Additional commenting to describe the scaling
        output += spacing + "// Range of " + name + " is " + getNumberString(encodedMin) + " to " + getNumberString(encodedMax) +  ".\n";

        if(encodedType.bits > 32)
        {
            if(support.longbitfield)
            {
                if(support.float64)
                    decodestring = "float64ScaledFromLongBitfield(" + decodestring;
                else
                    decodestring = "float32ScaledFromLongBitfield(" + decodestring;
            }
            else
            {
                if(support.float64)
                    decodestring = "float64ScaledFromBitfield(" + decodestring;
                else
                    decodestring = "float32ScaledFromBitfield(" + decodestring;
            }
        }
        else
            decodestring = "float32ScaledFromBitfield(" + decodestring;

        decodestring += ", " + getNumberString(encodedMin, encodedType.bits);
        decodestring += ", " + getNumberString(1.0, encodedType.bits) + "/" + getNumberString(scaler, encodedType.bits);
        decodestring += ")";
    }

    if(inMemoryType.isNull)
    {
        if(comment.isEmpty())
            output += spacing + "// reserved bits\n";

        if(checkConstant)
        {
            output += spacing + "// Decoded value must be " + constantstring + "\n";
            output += spacing + "if (" + decodestring + " != " + constantstring + ")\n";
            output += spacing + "    return 0;\n";
        }
        else
            output += spacing + decodestring + ";\n";
    }
    else
    {
        QString lhs;
        if(isStructureMember)
            lhs = "user->"; // Access via structure pointer
        else
            lhs = "*";      // Access via direct pointer

        // we cast here, because the inMemoryType might not be a unsigned int
        output += spacing + lhs + name + " = (" + typeName + ")" + decodestring + ";\n";

        if(checkConstant)
        {
            output += "\n";
            output += spacing + "// Decoded value must be " + constantstring + "\n";
            output += spacing + "if (" + lhs + name + " != " + constantstring + ")\n";
            output += spacing + "    return 0;\n";
        }
    }

    // Close the default block
    if(defaultEnabled && !defaultValue.isEmpty())
    {
        // Remove the last four spaces
        spacing.remove(spacing.size()-4, 4);
        output += spacing + "}\n";
    }

    *bitcount += encodedType.bits;

    return output;

}// ProtocolField::getDecodeStringForBitfield


/*!
 * Get the source needed to close out a string of bitfields.
 * \param bitcount points to the running count of bits in this string of
 *        bitfields, and will be updated by this function.
 * \return The string to add to the source file that closes the bitfield.
 */
QString ProtocolField::getCloseBitfieldString(int* bitcount) const
{
    QString output;
    QString spacing;

    if(*bitcount != 0)
    {
        // The number of bytes that the previous sequence of bit fields used up
        int length = *bitcount / 8;

        // Get the spacing right
        spacing += "    ";

        // If bitcount is not modulo 8, then the last byte was still in
        // progress, so increment past that
        if((*bitcount) % 8)
        {
            output += spacing + "bitcount = 0; byteindex++; // close bit field, go to next byte\n";
            length++;
        }
        else
        {
           output += spacing + "bitcount = 0; // close bit field, byte index already advanced\n";
        }

        output += "\n";

        // Reset bit counter
        *bitcount = 0;
    }

    return output;

}// ProtocolField::getCloseBitfieldString


/*!
 * Get the next lines of source needed to encode this string field.
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \return The string to add to the source file to that encodes this field
 */
QString ProtocolField::getEncodeStringForString(bool isStructureMember) const
{
    QString output;
    QString lhs;

    QString constantstring = getConstantString();

    if(encodedType.isNull)
        return output;

    if(isStructureMember)
        lhs = "user->";
    else
        lhs = "";

    if(!comment.isEmpty())
        output += "    // " + comment + "\n";

    if(constantstring.isEmpty())
        output += "    stringToBytes(" + lhs + name + ", data, &byteindex, " + array;
    else
        output += "    stringToBytes(" + constantstring + ", data, &byteindex, " + array;

    if(inMemoryType.isFixedString)
        output += ", 1);\n";
    else
        output += ", 0);\n";

    return output;

}// ProtocolField::getEncodeStringForString


/*!
 * Get the next lines of source needed to decode this string field
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \return The string to add to the source file to that decodes this field
 */
QString ProtocolField::getDecodeStringForString(bool isStructureMember) const
{
    QString output;

    if(encodedType.isNull)
        return output;

    QString lhs;

    if(isStructureMember)
        lhs = "user->";
    else
        lhs = "";

    if(!comment.isEmpty())
        output += "    // " + comment + "\n";

    output += "    stringFromBytes(" + lhs + name + ", data, &byteindex, " + array;

    if(inMemoryType.isFixedString)
        output += ", 1);\n";
    else
        output += ", 0);\n";

    if(checkConstant)
    {
        QString constantstring = getConstantString();
        output += "\n";
        output += "    // Decoded value must be " + constantstring + "\n";
        output += "    if (strncmp(" + lhs + name + ", " + constantstring + ", " + array + ") != 0)\n";
        output += "        return 0;\n";
    }

    return output;

}// ProtocolField::getDecodeStringForString


/*!
 * Return the string that is used to encode this structure.
 * \param isStructureMember is true if this encodable is accessed by structure pointer
 * \return the string to add to the source to encode this structure
 */
QString ProtocolField::getEncodeStringForStructure(bool isStructureMember) const
{
    QString output;
    QString access;
    QString spacing = "    ";

    if(encodedType.isNull)
        return output;

    if(!comment.isEmpty())
        output += spacing + "// " + comment + "\n";

    if(!dependsOn.isEmpty())
    {
        if(isStructureMember)
            output += spacing + "if(user->" + dependsOn + ")\n";
        else
            output += spacing + "if(" + dependsOn + ")\n";
        output += spacing + "{\n";
        spacing += "    ";
    }

    if(isArray())
    {
        if(variableArray.isEmpty())
        {
            output += spacing + "for(i = 0; i < " + array + "; i++)\n";
        }
        else
        {
            if(isStructureMember)
                output += spacing + "for(i = 0; i < (int)user->" + variableArray + " && i < " + array + "; i++)\n";
            else
                output += spacing + "for(i = 0; i < (int)" + variableArray + " && i < " + array + "; i++)\n";
        }


        if(is2dArray())
        {
            if(variable2dArray.isEmpty())
            {
                output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
            }
            else
            {
                if(isStructureMember)
                    output += spacing + "    for(j = 0; j < (int)user->" + variable2dArray + " && j < " + array2d + "; j++)\n";
                else
                    output += spacing + "    for(j = 0; j < (int)" + variable2dArray + " && j < " + array2d + "; j++)\n";
            }

            if(isStructureMember)
                access = "&user->" + name + "[i][j]";
            else
                access = "&" + name + "[i][j]";

            output += spacing + "        encode" + typeName + "(data, &byteindex, " + access + ");\n";
        }
        else
        {
            if(isStructureMember)
                access = "&user->" + name + "[i]";
            else
                access = "&" + name + "[i]";

            output += spacing + "    encode" + typeName + "(data, &byteindex, " + access + ");\n";
        }
    }
    else
    {
        if(isStructureMember)
            access = "&user->" + name;
        else
            access = name;  // in this case, name is already pointer, so we don't need "&"

        output += spacing + "encode" + typeName + "(data, &byteindex, " + access + ");\n";
    }

    if(!dependsOn.isEmpty())
        output += "    }\n";

    return output;

}// ProtocolField::getEncodeStringForStructure


/*!
 * Get the next lines of source needed to decode this external structure field
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \return The string to add to the source file to that decodes this field
 */
QString ProtocolField::getDecodeStringForStructure(bool isStructureMember) const
{
    QString output;
    QString access;
    QString spacing = "    ";

    if(encodedType.isNull)
        return output;

    if(!comment.isEmpty())
        output += "    // " + comment + "\n";

    if(!dependsOn.isEmpty())
    {
        if(isStructureMember)
            output += spacing + "if(user->" + dependsOn + ")\n";
        else
            output += spacing + "if(" + dependsOn + ")\n";
        output += spacing + "{\n";
        spacing += "    ";
    }

    if(isArray())
    {
        if(variableArray.isEmpty())
            output += spacing + "for(i = 0; i < " + array + "; i++)\n";
        else
        {
            if(isStructureMember)
                output += spacing + "for(i = 0; i < (int)user->" + variableArray + " && i < " + array + "; i++)\n";
            else
                output += spacing + "for(i = 0; i < (int)(*" + variableArray + ") && i < " + array + "; i++)\n";
        }

        output += spacing + "{\n";

        if(is2dArray())
        {
            if(variable2dArray.isEmpty())
                output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
            else
            {
                if(isStructureMember)
                    output += spacing + "    for(j = 0; j < (int)user->" + variable2dArray + " && j < " + array2d + "; j++)\n";
                else
                    output += spacing + "    for(j = 0; j < (int)(*" + variable2dArray + ") && j < " + array2d + "; j++)\n";
            }

            output += spacing + "    {\n";

            if(isStructureMember)
                access = "&user->" + name + "[i][j]";
            else
                access = "&" + name + "[i][j]";

            output += spacing + "        if(decode" + typeName + "(data, &byteindex, " + access + ") == 0)\n";
            output += spacing + "            return 0;\n";
            output += spacing + "    }\n";
            output += spacing + "}\n";
        }
        else
        {
            if(isStructureMember)
                access = "&user->" + name + "[i]";
            else
                access = "&" + name + "[i]";

            output += spacing + "    if(decode" + typeName + "(data, &byteindex, " + access + ") == 0)\n";
            output += spacing + "        return 0;\n";
            output += spacing + "}\n";
        }

    }
    else
    {
        if(isStructureMember)
            access = "&user->" + name;
        else
            access = name;  // in this case, name is already pointer, so we don't need "&"

        output += spacing + "if(decode" + typeName + "(data, &byteindex, " + access + ") == 0)\n";
        output += spacing + "    return 0;\n";
    }

    if(!dependsOn.isEmpty())
        output += "    }\n";

    return output;

}// ProtocolField::getDecodeStringForStructure


/*!
 * Look for a constantValue, in order of preference:
 * 1. constantValue
 * 2. If in memory type is null use "0"
 * \return the constant value
 */
QString ProtocolField::getConstantString(void) const
{
    if (!constantValue.isEmpty())
    {
        if(encodedType.isString)
        {
            // constantValue is a string literal, so include the quotes. Except for
            // a special case. If constantValue ends in "()" then we assume its a
            // function or macro call
            if(constantValue.contains("(") && constantValue.contains(")"))
                return constantValue;
            else
                return "\"" + constantValue + "\"";
        }
        else
            return constantValue;
    }
    else if(inMemoryType.isNull)
    {
        if(encodedType.isString)
        {
            // A zero with quotes around it
            return "\"0\"";
        }
        else
            return "0";
    }
    else
        return QString();

}// ProtocolField::getConstantString


/*!
 * Get the next lines(s) of source coded needed to encode this field, which
 * is not a bitfield or a string
 * \param isBigEndian should be true for big endian encoding.
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \return The string to add to the source file that encodes this field.
 */
QString ProtocolField::getEncodeStringForField(bool isBigEndian, bool isStructureMember) const
{
    QString output;
    QString endian;
    QString lengthString;
    QString lhs;
    QString constantstring = getConstantString();

    if(encodedType.isNull)
        return output;

    if(isStructureMember)
        lhs = "user->";
    else
        lhs = "";

    if(!comment.isEmpty())
        output += "    // " + comment + "\n";

    int length = encodedType.bits / 8;
    lengthString.setNum(length);

    // Remember that we could be encoding an array
    if(isArray())
        lengthString += "*" + array;

    if(is2dArray())
        lengthString += "*" + array2d;

    // The endian string, which is empty for 1 byte, since
    // endian only applies to multi-byte fields
    if(length > 1)
    {
        if(isBigEndian)
            endian += "Be";
        else
            endian += "Le";
    }
    else
        endian = "";

    QString spacing = "    ";

    if(!dependsOn.isEmpty())
    {
        if(isStructureMember)
            output += spacing + "if(user->" + dependsOn + ")\n";
        else
            output += spacing + "if(" + dependsOn + ")\n";
        output += spacing + "{\n";
        spacing += "    ";
    }

    if(encodedType.isFloat)
    {
        // In this case we are encoding as a floating point. Typically we
        // would not scale here, but there are cases where scaling is
        // interesting.
        QString scalestring;

        // Notice that encodedMax and encodedMin do not make sense since
        // the encoded type is float
        if(scaler != 1.0)
        {
            scalestring = "*" + getNumberString(scaler, inMemoryType.bits);

            // Additional commenting to describe the scaling
            output += spacing + "// " + name + " is scaled by " + getNumberString(scaler) +  ".\n";

        }

        // Notice that we have to cast to the
        // input parameter type, since the user might (for example) have
        // the in-memory type as a double, but the encoded as a float
        QString cast = "(" + encodedType.toTypeString() + ")";

        if(array.isEmpty())
        {
            if(constantstring.isEmpty())
                output += spacing + "float" + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + lhs + name + scalestring + ", data, &byteindex);\n";
            else
                output += spacing + "float" + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + constantstring + ", data, &byteindex);\n";
        }
        else
        {
            if(variableArray.isEmpty())
                output += spacing + "for(i = 0; i < " + array + "; i++)\n";
            else
                output += spacing + "for(i = 0; i < (int)" + lhs + variableArray + " && i < " + array + "; i++)\n";

            if(is2dArray())
            {
                if(variable2dArray.isEmpty())
                    output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
                else
                    output += spacing + "    for(j = 0; j < (int)" + lhs + variable2dArray + " && j < " + array2d + "; j++)\n";

                if(constantstring.isEmpty())
                    output += spacing + "        float" + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + lhs + name +  scalestring + "[i][j], data, &byteindex);\n";
                else
                    output += spacing + "        float" + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + constantstring + ", data, &byteindex);\n";
            }
            else
            {
                if(constantstring.isEmpty())
                    output += spacing + "    float" + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + lhs + name +  scalestring + "[i], data, &byteindex);\n";
                else
                    output += spacing + "    float" + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + constantstring + ", data, &byteindex);\n";
            }
        }
    }
    else if(encodedMax > encodedMin)
    {
        // The scaled outputs

        // Additional commenting to describe the scaling
        output += spacing + "// Range of " + name + " is " + getNumberString(encodedMin) + " to " + getNumberString(encodedMax) +  ".\n";

        // Handle the array
        if(!array.isEmpty())
        {
            if(variableArray.isEmpty())
                output += spacing + "for(i = 0; i < " + array + "; i++)\n";
            else
                output += spacing + "for(i = 0; i < (int)" + lhs + variableArray + " && i < " + array + "; i++)\n";

            if(is2dArray())
            {
                if(variable2dArray.isEmpty())
                    output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
                else
                    output += spacing + "    for(j = 0; j < (int)" + lhs + variable2dArray + " && j < " + array2d + "; j++)\n";

                // indent the next line
                output += "    ";

            }

            // indent the next line
            output += "    ";
        }

        output += spacing;

        // If we are scaling, then we are going to use a float-encoding
        // function, since even an integer encoding function would still
        // have to cast to float to apply the scaler. Note that
        if((inMemoryType.bits > 32) && support.float64)
            output += "float64";
        else
            output += "float32";

        output += "ScaledTo";

        // Now the encoded part:

        // Number of bytes
        output += QString().setNum(length);

        // Signed or unsigned
        if(encodedType.isSigned)
            output += "Signed";
        else
            output += "Unsigned";

        // More of the encode call signature, including endian
        output += endian + "Bytes(";

        // Check to see if we need to cast to float
        if(!inMemoryType.isFloat)
        {
            if((inMemoryType.bits > 32) && support.float64)
                output += "(double)";
            else
                output += "(float)";
        }

        // Scaling a constant would be really weird...
        if(constantstring.isEmpty())
        {
            // The reference to the data
            output += lhs + name;

            if(is2dArray())
                output += "[i][j]";
            else if(isArray())
                output += "[i]";
        }
        else
            output += constantstring;

        output += ", data, &byteindex";

        // Signature changes for signed versus unsigned
        if(!encodedType.isSigned)
            output += ", " + getNumberString(encodedMin, inMemoryType.bits);

        output +=  ", " + getNumberString(scaler, inMemoryType.bits);

        output += ");\n";

    }// if scaled to integer
    else
    {
        // Here we are not scaling, and we are not encoding a float. It may
        // be that the encoded type is the same as the in-memory, but in
        // case it is not we add a cast.
        QString cast = "(" + encodedType.toTypeString() + ")";
        QString opener;

        if(encodedType.isSigned)
        {
            opener = "int";
        }
        else
        {
            opener = "uint";
        }

        if(array.isEmpty())
        {
            if(constantstring.isEmpty())
                output += spacing + opener + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + lhs + name + ", data, &byteindex);\n";
            else
                output += spacing + opener + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + constantstring + ", data, &byteindex);\n";
        }
        else
        {
            if(variableArray.isEmpty())
                output += spacing + "for(i = 0; i < " + array + "; i++)\n";
            else
                output += spacing + "for(i = 0; i < (int)" + lhs + variableArray + " && i < " + array + "; i++)\n";

            if(is2dArray())
            {
                if(variable2dArray.isEmpty())
                    output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
                else
                    output += spacing + "    for(j = 0; j < (int)" + lhs + variable2dArray + " && j < " + array2d + "; j++)\n";

                if(constantstring.isEmpty())
                    output += spacing + "        " + opener + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + lhs + name + "[i][j], data, &byteindex);\n";
                else
                    output += spacing + "        " + opener + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + constantstring + ", data, &byteindex);\n";
            }
            else
            {
                if(constantstring.isEmpty())
                    output += spacing + "    " + opener + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + lhs + name + "[i], data, &byteindex);\n";
                else
                    output += spacing + "    " + opener + QString().setNum(encodedType.bits) + "To" + endian + "Bytes(" + cast + constantstring + ", data, &byteindex);\n";
            }
        }

    }// else not scaled and not float

    if(!dependsOn.isEmpty())
        output += "    }\n";

    return output;

}// ProtocolField::getEncodeStringForField


/*!
 * Get the next lines(s) of source coded needed to decode this field, which
 * is not a bitfield or a string
 * \param isBigEndian should be true if the protocol uses big endian ordering.
 * \param isStructureMember should be true if the left hand side is a
 *        member of a user structure, else the left hand side is a pointer
 *        to the inMemoryType
 * \param defaultEnabled should be true to enable default handling
 * \return The string to add to the source file that decodes this field.
 */
QString ProtocolField::getDecodeStringForField(bool isBigEndian, bool isStructureMember, bool defaultEnabled) const
{
    QString output;
    QString endian;
    QString spacing = "    ";
    QString lhs;

    QString constantstring = getConstantString();

    if(encodedType.isNull)
        return output;

    if(isStructureMember)
        lhs = "user->"; // member of a structure
    else if(array.isEmpty())
        lhs = "*";      // direct pointer
    else
        lhs = "";       // pointer with array de-referencing

    if(!comment.isEmpty())
        output += spacing + "// " + comment + "\n";

    int length = encodedType.bits / 8;

    // The endian string, which is empty for 1 byte, since
    // endian only applies to multi-byte fields
    if(length > 1)
    {
        if(isBigEndian)
            endian += "Be";
        else
            endian += "Le";
    }
    else
        endian = "";

    QString lengthString;

    // What is the length in bytes of this field, remember that we could be encoding an array
    lengthString.setNum(length);

    if(isArray())
        lengthString += "*" + array;

    if(is2dArray())
        lengthString += "*" + array2d;

    if(!dependsOn.isEmpty())
    {
        if(isStructureMember)
            output += spacing + "if(user->" + dependsOn + ")\n";
        else
            output += spacing + "if(" + dependsOn + ")\n";
        output += spacing + "{\n";
        spacing += "    ";
    }

    // If this field has a default value
    if(defaultEnabled && !defaultValue.isEmpty())
    {
        output += spacing + "if(byteindex + " + lengthString + " > numBytes)\n";
        output += spacing + "    return 1;\n";
        output += spacing + "else\n";
        output += spacing + "{\n";
        spacing += "    ";
    }

    if(inMemoryType.isNull)
    {
        // Skip over reserved space
        if(!array.isEmpty())
        {
            QString math = EncodedLength::collapseLengthString(lengthString, true);

            output += spacing + "byteindex += " + math + ";\n";
        }
        else if (checkConstant)
        {
            output += spacing + "// Decoded value must be " + constantstring + "\n";

            output += spacing + "if (";

            if (encodedType.isFloat)
            {
                if((inMemoryType.bits > 32) && support.float64)
                    output += "float64";
                else
                    output += "float32";
            }
            else if (encodedType.isSigned)
                output += "int";
            else
                output += "uint";

            output += QString().setNum(encodedType.bits) + "From" + endian + "Bytes(data, &byteindex)" + " != (" + encodedType.toTypeString() + ") " + constantstring + ")\n";
            output += spacing + "    return 0;\n";
        }
        else
        {
            output += spacing + "byteindex += " + lengthString + ";\n";
        }

    }
    else if(encodedType.isFloat)
    {
        // In this case we are encoding as a floating point. Typically we
        // would not scale here, but there are cases where scaling is
        // interesting.
        QString scalestring;

        // Notice that encodedMax and encodedMin do not make sense since
        // the encoded type is float
        if(scaler != 1.0)
        {
            scalestring = "(" + getNumberString(1.0, inMemoryType.bits) + "/" + getNumberString(scaler, inMemoryType.bits) + ")*" ;

            // Additional commenting to describe the scaling
            output += spacing + "// " + name + " is scaled by " + getNumberString(scaler) +  ".\n";
        }

        if(array.isEmpty())
            output += spacing + lhs + name + " = " + scalestring + "float" + QString().setNum(encodedType.bits) + "From" + endian + "Bytes(data, &byteindex);\n";
        else
        {
            if(variableArray.isEmpty())
                output += spacing + "for(i = 0; i < " + array + "; i++)\n";
            else
            {
                if(isStructureMember)
                    output += spacing + "for(i = 0; i < (int)user->" + variableArray + " && i < " + array + "; i++)\n";
                else
                    output += spacing + "for(i = 0; i < (int)(*" + variableArray + ") && i < " + array + "; i++)\n";
            }

            if(is2dArray())
            {
                if(variable2dArray.isEmpty())
                    output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
                else
                {
                    if(isStructureMember)
                        output += spacing + "    for(j = 0; j < (int)user->" + variable2dArray + " && j < " + array2d + ";j++)\n";
                    else
                        output += spacing + "    for(j = 0; j < (int)(*" + variable2dArray + ") && j < " + array2d + "; j++)\n";
                }

                output += spacing + "        " + lhs + name + "[i][j] = " + scalestring + "float" + QString().setNum(encodedType.bits) + "From" + endian + "Bytes(data, &byteindex);\n";
            }
            else
                output += spacing + "    " + lhs + name + "[i] = " + scalestring + "float" + QString().setNum(encodedType.bits) + "From" + endian + "Bytes(data, &byteindex);\n";

        }// if array

    }// if float
    else if(encodedMax > encodedMin)
    {
        // Additional commenting to describe the scaling
        output += spacing + "// Range of " + name + " is " + getNumberString(encodedMin) + " to " + getNumberString(encodedMax) +  ".\n";

        // Handle the array
        if(isArray())
        {
            if(variableArray.isEmpty())
                output += spacing + "for(i = 0; i < " + array + "; i++)\n";
            else
                output += spacing + "for(i = 0; i < (int)" + lhs + variableArray + " && i < " + array + "; i++)\n";

            if(is2dArray())
            {
                if(variable2dArray.isEmpty())
                    output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
                else
                    output += spacing + "    for(j = 0; j < (int)" + lhs + variable2dArray + " && j < " + array2d + "; j++)\n";

                // start the next line
                output += spacing + "        " + lhs + name + "[i][j] = ";
            }
            else
            {
                // start the next line
                output += spacing + "    " + lhs + name + "[i] = ";
            }
        }
        else
            output += spacing + lhs + name + " = ";

        // The cast if the in memory type is not floating
        if(!inMemoryType.isFloat)
        {
            if(inMemoryType.isSigned)
                output += "(int" + QString().setNum(inMemoryType.bits) + "_t)";
            else
                output += "(uint" + QString().setNum(inMemoryType.bits) + "_t)";
        }

        if((inMemoryType.bits > 32) && support.float64)
            output += "float64";
        else
            output += "float32";

        output += "ScaledFrom";

        // Now the encoded part:

        // Number of bytes
        output += QString().setNum(length);

        // Signed or unsigned
        if(encodedType.isSigned)
            output += "Signed";
        else
            output += "Unsigned";

        output += endian + "Bytes(data, &byteindex";

        // Signature changes for signed versus unsigned
        if(!encodedType.isSigned)
            output += ", " + getNumberString(encodedMin, inMemoryType.bits);

        output +=  ", " + getNumberString(1.0, inMemoryType.bits) + "/" + getNumberString(scaler, inMemoryType.bits);

        output +=  ");\n";

    }// if scaled to integer
    else
    {
        // Here we are not scaling, and we are not encoding a float. It may
        // be that the encoded type is the same as the in-memory, but in
        // case it is not we add a cast.
        QString cast = "(" + typeName + ")";
        QString opener;

        if(encodedType.isSigned)
            opener = "int";
        else
            opener = "uint";

        if(array.isEmpty())
            output += spacing + lhs + name + " = " + cast + opener + QString().setNum(encodedType.bits) + "From" + endian + "Bytes(data, &byteindex);\n";
        else
        {
            if(variableArray.isEmpty())
                output += spacing + "for(i = 0; i < " + array + "; i++)\n";
            else
            {
                if(isStructureMember)
                    output += spacing + "for(i = 0; i < (int)user->" + variableArray + " && i < " + array + "; i++)\n";
                else
                    output += spacing + "for(i = 0; i < (int)(*" + variableArray + ") && i < " + array + "; i++)\n";
            }

            if(is2dArray())
            {
                if(variable2dArray.isEmpty())
                    output += spacing + "    for(j = 0; j < " + array2d + "; j++)\n";
                else
                {
                    if(isStructureMember)
                        output += spacing + "    for(j = 0; j < (int)user->" + variable2dArray + " && j < " + array2d + "; j++)\n";
                    else
                        output += spacing + "    for(j = 0; j < (int)(*" + variable2dArray + ") && j < " + array2d + "; j++)\n";
                }

                output += spacing + "        " + lhs + name + "[i][j] = " + cast + opener + QString().setNum(encodedType.bits) + "From" + endian + "Bytes(data, &byteindex);\n";
            }
            else
                output += spacing + "    " + lhs + name + "[i] = " + cast + opener + QString().setNum(encodedType.bits) + "From" + endian + "Bytes(data, &byteindex);\n";
        }

    }// else not scaled and not float

    // Handle the check constant case, the null case was handled above
    if(!inMemoryType.isNull && checkConstant && array.isEmpty())
    {
        output += "\n";
        output += spacing + "// Decoded value must be " + constantstring + "\n";
        output += spacing + "if (" + lhs + name + " != " + constantstring + ")\n";
        output += spacing + spacing + "return 0;\n";
    }

    // Close the default block
    if(defaultEnabled && !defaultValue.isEmpty())
    {
        // Remove the last four spaces
        spacing.remove(spacing.size()-4, 4);
        output += spacing + "}\n";
    }

    // Close the depends on block
    if(!dependsOn.isEmpty())
    {
        // Remove the last four spaces
        spacing.remove(spacing.size()-4, 4);
        output += spacing + "}\n";
    }

    return output;

}// ProtocolField::getDecodeStringForField


/*!
 * Get a properly formatted number string for a floating point number
 * \param number is the number to turn into a string
 * \param bits is the number of bits in memory for this string. 32 or less will prompt a 'f' suffix on the string
 * \return the string.
 */
QString ProtocolField::getNumberString(double number, int bits) const
{
    QString string;

    string.setNum(number, 'g', 16);

    // Make sure we have a decimal point
    if(!string.contains("."))
        string += ".0";

    // Float suffix
    if((bits <= 32) || !support.float64)
        string += "f";

    return string;
}


/*!
 * Get a properly formatted number string for a floating point number. If the
 * number is one of these multiples of pi (-2, -1, -.5, .5, 1, 2), then return
 * a string that includes the html pi token.
 * \param number is the number to turn into a string
 * \return the string.
 */
QString ProtocolField::getDisplayNumberString(double number)
{
    if(number == -2*3.14159265358979323846)
    {
        return "-2&pi;";
    }
    else if(number == -3.14159265358979323846)
    {
        return "-&pi;";
    }
    else if(number == -0.5*3.14159265358979323846)
    {
        return "-&pi;/2";
    }
    else if(number == 0.5*3.14159265358979323846)
    {
        return "&pi;/2";
    }
    else if(number == 3.14159265358979323846)
    {
        return "&pi;";
    }
    else if(number == 2*3.14159265358979323846)
    {
        return "2&pi;";
    }
    else
    {
        QString string;

        string.setNum(number, 'g', 16);

        // Make sure we have a decimal point
        if(!string.contains("."))
            string += ".0";

        return string;
    }
}


/*!
 * Compute the power of 2 raised to some bits
 * \param bits is the number of bits to raise to
 * \return the power of 2 raised to bits.
 */
uint64_t ProtocolField::pow2(uint8_t bits) const
{
    uint64_t output = 2;

    if(bits == 0)
        return 1;

    // If bits is 1, then 2 is output, if bits is 3 then 2 multiplies occur, yeilding 8
    for(int i = 1; i < bits; i++)
        output *= 2;

    return output;
}

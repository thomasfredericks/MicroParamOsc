#ifndef MICRO_PARAM_OSC_H
#define MICRO_PARAM_OSC_H
#include <MicroParam.h>
#include <MicroOsc.h>

/// MicroParamOscBind
/// Binds one or multiple MicroParam objects to an OSC address and type tags.
/// Allows matching incoming OSC messages and sending parameter values via OSC.
class MicroParamOscBind
{
private:
    const char *address_;  ///< OSC address for this binding
    uint32_t addressHash_; ///< Hash of the OSC address for fast matching

    const char *typeTags_;  ///< OSC type tags describing parameter types
    uint32_t typeTagsHash_; ///< Hash of the type tags for fast matching

    MicroParam *singleParam_[1]; ///< Storage for a single parameter
    MicroParam **params_;        ///< Pointer to array of parameters
    size_t count_;               ///< Number of parameters bound

public:
    /// Generates a hash for a given string using FNV-1a
    /// @param s String to hash (`const char *`)
    /// @return Hash value (`uint32_t`)
    static uint32_t generateHash(const char *s)
    {
        return microHashFnv1a(s);
    }

    // -------- Constructors --------

    /// Constructor for a single parameter binding
    /// @param address OSC address (`const char *`)
    /// @param typeTags OSC type tags (`const char *`)
    /// @param param Reference to a MicroParam (`MicroParam &`)
    MicroParamOscBind(const char *address,
                      const char *typeTags,
                      MicroParam &param)
        : address_(address),
          addressHash_(generateHash(address)),
          typeTags_(typeTags),
          typeTagsHash_(generateHash(typeTags)),
          count_(1)
    {
        singleParam_[0] = &param;
        params_ = singleParam_;
    }

    /// Constructor for an array of parameter bindings
    /// @param address OSC address (`const char *`)
    /// @param typeTags OSC type tags (`const char *`)
    /// @param paramArray Array of MicroParam pointers (`MicroParam **`)
    MicroParamOscBind(const char *address,
                      const char *typeTags,
                      MicroParam **paramArray)
        : address_(address),
          addressHash_(generateHash(address)),
          typeTags_(typeTags),
          typeTagsHash_(generateHash(typeTags)),
          params_(paramArray)
    {
        count_ = strlen(typeTags);
    }

    // -------- Accessors --------

    /// Returns the OSC address for this binding
    const char *getAddress() const { return address_; }

    /// Returns the hash of the OSC address
    uint32_t getAddressHash() const { return addressHash_; }

    /// Returns the OSC type tags
    const char *getTypeTags() const { return typeTags_; }

    /// Returns the hash of the OSC type tags
    uint32_t getTypeTagsHash() const { return typeTagsHash_; }

    /// Returns the number of parameters in this binding
    size_t getCount() const { return count_; }

    /// Returns the first MicroParam in this binding
    MicroParam &getParam() const { return *params_[0]; }

    /// Returns the MicroParam at the given index
    /// @param index Index of the parameter (`size_t`)
    MicroParam &getParam(size_t index) const { return *params_[index]; }

    // -------- Matching --------

    /// Checks if the binding's address matches the given string
    /// @param addr OSC address to compare (`const char *`)
    /// @return True if addresses match
    bool matchesAddress(const char *addr) const { return strcmp(address_, addr) == 0; }

    /// Checks if the binding's address hash matches the given hash
    /// @param hash Hash to compare (`uint32_t`)
    /// @return True if hashes match
    bool matchesAddressHash(uint32_t hash) const { return addressHash_ == hash; }

    /// Checks if the type tags hash matches the given hash
    /// @param hash Hash to compare (`uint32_t`)
    /// @return True if hashes match
    bool matchesTypeTagsHash(uint32_t hash) const { return typeTagsHash_ == hash; }

    /// Checks if the type tags match the given string
    /// @param tags OSC type tags to compare (`const char *`)
    /// @return True if type tags match
    bool matchesTypeTags(const char *tags) const { return (strcmp(typeTags_, tags) == 0); }
};

// ======================= MicroParam OSC Utilities =======================

/// Dispatches an incoming OSC message to the matching bindings
/// @param message Incoming OSC message (`MicroOscMessage &`)
/// @param bindings Array of bindings (`MicroParamOscBind *`)
/// @param bindingsCount Number of bindings (`size_t`)
/// @return True if a binding was updated successfully
bool microParamOscDispatch(MicroOscMessage &message,
                           MicroParamOscBind *bindings,
                           size_t bindingsCount)
{
    const char *address = message.getOscAddress();
    const uint32_t addrHash = MicroParamOscBind::generateHash(address);

    const char *oscTypeTags = message.getTypeTags();
    const uint32_t oscTypeHash = MicroParamOscBind::generateHash(oscTypeTags);

    for (size_t b = 0; b < bindingsCount; ++b)
    {
        MicroParamOscBind &binding = bindings[b];

        if (!binding.matchesAddressHash(addrHash) ||
            !binding.matchesTypeTagsHash(oscTypeHash))
            continue;

        for (size_t i = 0; i < binding.getCount(); ++i)
        {
            MicroParam &param = binding.getParam(i);
            char oscTag = oscTypeTags[i];

            switch (oscTag)
            {
            case 'i':
                // if (param.getType() == MicroParam::Type::Enum ||
                //     param.getType() == MicroParam::Type::Int ||
                //     param.getType() == MicroParam::Type::Byte)
                // {
                param.setInt(message.nextAsInt());
                // }
                // else
                // {
                //     return false;
                // }
                break;

            case 'f':
                // if (param.getType() == MicroParam::Type::Float ||
                //     param.getType() == MicroParam::Type::Int)
                // {
                param.setFloat(message.nextAsFloat());
                // }
                // else
                // {
                //     return false;
                // }
                break;

            case 's':
                // if (param.getType() == MicroParam::Type::String ||
                //     param.getType() == MicroParam::Type::Enum)
                // {
                param.setString(message.nextAsString());
                // }
                // else
                // {
                //     return false;
                // }
                break;

            case 'b': // blob / array
            {
                // if (param.getType() == MicroParam::Type::Blob)
                // {
                const uint8_t *data;
                uint32_t length = message.nextAsBlob(&data);
                param.setBlob(data, length);
                // }
                // else
                // {
                //     return false;
                // }
            }
            break;

            default:
                return false;
            }
        }

        return true; // binding matched and applied
    }

    return false;
}

/// Sends a single binding's parameters via OSC
/// @param osc OSC instance (`MicroOsc &`)
/// @param binding Binding to send (`MicroParamOscBind &`)
void microParamOscSend(MicroOsc &osc, MicroParamOscBind &binding)
{
    const char *address = binding.getAddress();
    const char *tags = binding.getTypeTags();
    size_t count = binding.getCount();

    osc.messageBegin(address, tags);

    for (size_t i = 0; i < count; ++i)
    {
        MicroParam &param = binding.getParam(i);
        char tag = tags[i];

        switch (tag)
        {
        case 'i':
            osc.messageAddInt(param.getInt());
            break;

        case 'f':
            osc.messageAddFloat(param.getFloat());
            break;

        case 's':
            osc.messageAddString(param.getString());
            break;

        case 'b':
        {
            uint8_t *data;
            size_t length = param.getBlob(&data);
            osc.messageAddBlob(data, length);
            break;
        }

        default:
            break;
        }
    }

    osc.messageEnd();
}

/// Sends multiple bindings via OSC
/// @param osc OSC instance (`MicroOsc &`)
/// @param bindings Array of bindings (`MicroParamOscBind *`)
/// @param bindingsCount Number of bindings (`size_t`)
void microParamOscSend(MicroOsc &osc, MicroParamOscBind *bindings, size_t bindingsCount)
{
    for (size_t p = 0; p < bindingsCount; ++p)
    {
        microParamOscSend(osc, bindings[p]);
    }
}
#endif // MICRO_PARAM_OSC_H
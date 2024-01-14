#include <iostream>
#include <tuple>
#include <array>
#include <optional>

enum class Categhory : std::uint8_t {
    Millitary = 1,
    Commerce,
    Passenger
};

enum class Dimension : std::uint8_t {
    Huge = 1,
    Large,
    Medium,
    Small,
    Tiny
};

struct SpaceCraft {
    std::size_t mUniqueId;
    Categhory mCateghory;
    Dimension mDimension;
};

template <std::size_t... N>
struct Range {};

template <std::size_t N, std::size_t... other>
struct MakeRange : MakeRange<N - 1, N - 1, other...> {};

template <std::size_t... other>
struct MakeRange<0, other...> {
    using resultRange = Range<other...>;
};

auto presentAsTuple(std::size_t& metaInfo) noexcept {
    return std::tie(metaInfo);
}

auto presentAsTuple(const std::size_t& metaInfo) noexcept {
    return std::tie(metaInfo);
}

auto presentAsTuple(SpaceCraft &sp) noexcept {
    return std::tie(sp.mUniqueId, sp.mCateghory, sp.mDimension);
}

auto presentAsTuple(const SpaceCraft &sp) noexcept {
    return std::tie(sp.mUniqueId, sp.mCateghory, sp.mDimension);
}

template <typename T, std::size_t n>
constexpr auto findFieldImpl() noexcept {
    std::make_pair(sizeof(T), n);
}

template <typename... Types, std::size_t... range>
auto findFieldImpl(Range<range...>, std::size_t baseOffset) noexcept {
    constexpr auto tuple = std::make_tuple(std::remove_reference_t<Types>{}...);
    std::size_t currentSum = 0;
    for (std::size_t i = 0; i < sizeof...(Types); ++i) {
        (findFieldImpl<Types, range>(), ...);
    }
    return ;
}

template <typename... Types>
auto findField(std::tuple<Types...> &tuple, std::size_t baseOffsetField) noexcept {
    using R = typename MakeRange<sizeof...(Types)>::resultRange;
    return findFieldImpl<Types...>(R{}, baseOffsetField);
}

template <typename T>
constexpr std::size_t getSizeImpl(const T&) noexcept {
    return sizeof(T);
}

template <typename... Types>
constexpr std::size_t getSize(const std::tuple<Types...> &tuple) noexcept {
    return (0 + ... + getSizeImpl(Types{}));
}

namespace detail {
    constexpr static std::uint8_t maxFileSCRecords = 2;
class FtFileGeneral {
protected:
    std::array<std::uint8_t, sizeof(std::size_t)> mTempBuf{};
    std::size_t mFileIter = 0;
protected:
    std::array<SpaceCraft, maxFileSCRecords>* mData;
protected:
    template <typename iter>
    void readImpl(iter& data, std::size_t size) { 
        auto mTempBufIter = mTempBuf.begin();
        for (std::size_t i = 0; i < size; ++i) {
            *mTempBufIter = *data;
            ++mTempBufIter;
            ++data;
        }
    }
public:
    virtual void setFilePtr(std::size_t offset) = 0;
    virtual void write(const std::uint8_t* data, std::size_t size) = 0;
    virtual void read(std::uint8_t* data, std::size_t size) = 0;
    virtual bool checkAvailiableFileCapacity(std::size_t offsetT, std::size_t size) = 0;
    virtual bool checkAvailiableFileCapacity(std::size_t offsetT, std::size_t offsetTfield, std::size_t size) = 0;
};
}

class FtFile : public detail::FtFileGeneral {
protected:
    constexpr static std::size_t maxFileSize = 28;
    std::array<std::uint8_t, maxFileSize> mFile{};
    std::array<std::uint8_t, maxFileSize>::iterator mFilePtr = mFile.begin();
public:
    void setFilePtr(std::size_t offset) override {
        mFileIter = offset;
        mFilePtr = mFile.begin() + mFileIter;
    }
    void write(const std::uint8_t* data, std::size_t size) override {
        for (std::size_t i = 0; i < size; ++i) {
            *mFilePtr = *data;
            ++mFilePtr;
            ++data;
        }
        mFileIter += size;
    }
    void read(std::uint8_t* data, std::size_t size) override {
        readImpl(mFilePtr, size);
        auto tempBufIter = mTempBuf.begin();
        for (std::size_t i = 0; i < size; ++i) {
            *data = *tempBufIter;
            ++data;
            ++tempBufIter;
        }
        mFileIter += size;
    }
    bool checkAvailiableFileCapacity(std::size_t offsetT, std::size_t size) override {
        return (mFileIter + offsetT + size) <= maxFileSize;
    }
    bool checkAvailiableFileCapacity(std::size_t offsetT, std::size_t offsetTfield, std::size_t size) override { 
        std::size_t temp = mFileIter + offsetT + offsetTfield + size;
        return checkAvailiableFileCapacity(offsetT, size) && (temp <= maxFileSize);
    }
};

class Serializer {
protected:
    detail::FtFileGeneral& ftFile;
private:

    template <typename... Types>
    void serializeImpl(const std::tuple<Types...> &tuple) noexcept {
        std::apply
        (
            [&](const Types& ... tupleArgs){
                (ftFile.write(reinterpret_cast<const std::uint8_t*>(&tupleArgs), sizeof(tupleArgs)), ...);
            }, tuple
        );
    }

    template <typename... Types>
    void desirializeImpl(std::tuple<Types...> &tuple) noexcept {
        std::apply 
        (
            [&](Types&... tupleArgs) {
                (ftFile.read(reinterpret_cast<std::uint8_t*>(&tupleArgs), sizeof(tupleArgs)), ...);
            }
            , tuple
        );
    }

    template <typename T>
    void desirializeImpl(T& arg) noexcept {
        ftFile.read(reinterpret_cast<std::uint8_t*>(&arg), sizeof(T));
    }
public:
    explicit Serializer(detail::FtFileGeneral& file): ftFile(file) {}   
    explicit Serializer(detail::FtFileGeneral&& file): ftFile(file) {}

    template <typename T>
    bool serialize(const T& t, std::size_t baseOffset = 0) noexcept {
        auto tuple = presentAsTuple(t);
        if (!ftFile.checkAvailiableFileCapacity(baseOffset, getSize(tuple))){
            return false;
        }
        serializeImpl(tuple);
        return true;
    } 

    template <typename T>
    bool desirializeField(T&t, std::size_t baseoffsetT, std::size_t baseOffsetTField) noexcept {
        ftFile.setFilePtr(baseoffsetT);
        // if user provide baseOffset > max file size.
        if (!ftFile.checkAvailiableFileCapacity(baseoffsetT, baseOffsetTField, 0)){
            return false;
        }
        auto tuple = presentAsTuple(t);
        findField(tuple, baseOffsetTField);
        // desirializeImpl();
        return true;
    }

    template <typename T>
    inline bool desirialize(T&& t, std::size_t baseOffset) noexcept {
        return desirialize(t, baseOffset);
    }

    template <typename T>
    bool desirialize(T& t, std::size_t baseOffset = 0) noexcept {
        ftFile.setFilePtr(baseOffset);
        // if user provide baseOffset > max file size.
        if (!ftFile.checkAvailiableFileCapacity(baseOffset, 0)) {
            return false;
        }
        ftFile.setFilePtr(baseOffset);
        auto tuple = presentAsTuple(t);
        desirializeImpl(tuple);
        return true;
    }
};


int main() {
    FtFile ftFile;
    Serializer serializer{ftFile};

    std::size_t someMetaInfo = 19;
    SpaceCraft smallPassengerSpaceCraft24{24, Categhory::Passenger, Dimension::Small};
    SpaceCraft bigComerceSpaceCraft2{2, Categhory::Commerce, Dimension::Large};

    serializer.serialize(someMetaInfo);
    serializer.serialize(smallPassengerSpaceCraft24);
    serializer.serialize(bigComerceSpaceCraft2);

    SpaceCraft iDontKnowWhithSpaceCraftIRead;
    serializer.desirialize(iDontKnowWhithSpaceCraftIRead, 8);

    SpaceCraft iDontknowOneFieldOfSpaceCraft;
    serializer.desirializeField(iDontknowOneFieldOfSpaceCraft, 8, 8);

    return 0;
}
// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
// clang-format off
// automatically generated by the FlatBuffers compiler, do not modify


#pragma once

#include "flatbuffers/flatbuffers.h"

namespace HistoSchema {

struct ArrayUInt;

struct ArrayULong;

struct ArrayDouble;

struct ArrayFloat;

struct DimensionMetaData;

struct EventHistogram;

enum class Array : uint8_t {
  NONE = 0,
  ArrayUInt = 1,
  ArrayULong = 2,
  ArrayDouble = 3,
  ArrayFloat = 4,
  MIN = NONE,
  MAX = ArrayFloat
};

inline const Array (&EnumValuesArray())[5] {
  static const Array values[] = {
    Array::NONE,
    Array::ArrayUInt,
    Array::ArrayULong,
    Array::ArrayDouble,
    Array::ArrayFloat
  };
  return values;
}

inline const char * const *EnumNamesArray() {
  static const char * const names[] = {
    "NONE",
    "ArrayUInt",
    "ArrayULong",
    "ArrayDouble",
    "ArrayFloat",
    nullptr
  };
  return names;
}

inline const char *EnumNameArray(Array e) {
  const size_t index = static_cast<int>(e);
  return EnumNamesArray()[index];
}

template<typename T> struct ArrayTraits {
  static const Array enum_value = Array::NONE;
};

template<> struct ArrayTraits<ArrayUInt> {
  static const Array enum_value = Array::ArrayUInt;
};

template<> struct ArrayTraits<ArrayULong> {
  static const Array enum_value = Array::ArrayULong;
};

template<> struct ArrayTraits<ArrayDouble> {
  static const Array enum_value = Array::ArrayDouble;
};

template<> struct ArrayTraits<ArrayFloat> {
  static const Array enum_value = Array::ArrayFloat;
};

bool VerifyArray(flatbuffers::Verifier &verifier, const void *obj, Array type);
bool VerifyArrayVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types);

struct ArrayUInt FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "ArrayUInt";
  }
  enum {
    VT_VALUE = 4
  };
  const flatbuffers::Vector<uint32_t> *value() const {
    return GetPointer<const flatbuffers::Vector<uint32_t> *>(VT_VALUE);
  }
  flatbuffers::Vector<uint32_t> *mutable_value() {
    return GetPointer<flatbuffers::Vector<uint32_t> *>(VT_VALUE);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_VALUE) &&
           verifier.VerifyVector(value()) &&
           verifier.EndTable();
  }
};

struct ArrayUIntBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_value(flatbuffers::Offset<flatbuffers::Vector<uint32_t>> value) {
    fbb_.AddOffset(ArrayUInt::VT_VALUE, value);
  }
  explicit ArrayUIntBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ArrayUIntBuilder &operator=(const ArrayUIntBuilder &);
  flatbuffers::Offset<ArrayUInt> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ArrayUInt>(end);
    return o;
  }
};

inline flatbuffers::Offset<ArrayUInt> CreateArrayUInt(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<uint32_t>> value = 0) {
  ArrayUIntBuilder builder_(_fbb);
  builder_.add_value(value);
  return builder_.Finish();
}

inline flatbuffers::Offset<ArrayUInt> CreateArrayUIntDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<uint32_t> *value = nullptr) {
  return CreateArrayUInt(
      _fbb,
      value ? _fbb.CreateVector<uint32_t>(*value) : 0);
}

struct ArrayULong FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "ArrayULong";
  }
  enum {
    VT_VALUE = 4
  };
  const flatbuffers::Vector<uint64_t> *value() const {
    return GetPointer<const flatbuffers::Vector<uint64_t> *>(VT_VALUE);
  }
  flatbuffers::Vector<uint64_t> *mutable_value() {
    return GetPointer<flatbuffers::Vector<uint64_t> *>(VT_VALUE);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_VALUE) &&
           verifier.VerifyVector(value()) &&
           verifier.EndTable();
  }
};

struct ArrayULongBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_value(flatbuffers::Offset<flatbuffers::Vector<uint64_t>> value) {
    fbb_.AddOffset(ArrayULong::VT_VALUE, value);
  }
  explicit ArrayULongBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ArrayULongBuilder &operator=(const ArrayULongBuilder &);
  flatbuffers::Offset<ArrayULong> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ArrayULong>(end);
    return o;
  }
};

inline flatbuffers::Offset<ArrayULong> CreateArrayULong(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<uint64_t>> value = 0) {
  ArrayULongBuilder builder_(_fbb);
  builder_.add_value(value);
  return builder_.Finish();
}

inline flatbuffers::Offset<ArrayULong> CreateArrayULongDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<uint64_t> *value = nullptr) {
  return CreateArrayULong(
      _fbb,
      value ? _fbb.CreateVector<uint64_t>(*value) : 0);
}

struct ArrayDouble FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "ArrayDouble";
  }
  enum {
    VT_VALUE = 4
  };
  const flatbuffers::Vector<double> *value() const {
    return GetPointer<const flatbuffers::Vector<double> *>(VT_VALUE);
  }
  flatbuffers::Vector<double> *mutable_value() {
    return GetPointer<flatbuffers::Vector<double> *>(VT_VALUE);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_VALUE) &&
           verifier.VerifyVector(value()) &&
           verifier.EndTable();
  }
};

struct ArrayDoubleBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_value(flatbuffers::Offset<flatbuffers::Vector<double>> value) {
    fbb_.AddOffset(ArrayDouble::VT_VALUE, value);
  }
  explicit ArrayDoubleBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ArrayDoubleBuilder &operator=(const ArrayDoubleBuilder &);
  flatbuffers::Offset<ArrayDouble> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ArrayDouble>(end);
    return o;
  }
};

inline flatbuffers::Offset<ArrayDouble> CreateArrayDouble(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<double>> value = 0) {
  ArrayDoubleBuilder builder_(_fbb);
  builder_.add_value(value);
  return builder_.Finish();
}

inline flatbuffers::Offset<ArrayDouble> CreateArrayDoubleDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<double> *value = nullptr) {
  return CreateArrayDouble(
      _fbb,
      value ? _fbb.CreateVector<double>(*value) : 0);
}

struct ArrayFloat FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "ArrayFloat";
  }
  enum {
    VT_VALUE = 4
  };
  const flatbuffers::Vector<float> *value() const {
    return GetPointer<const flatbuffers::Vector<float> *>(VT_VALUE);
  }
  flatbuffers::Vector<float> *mutable_value() {
    return GetPointer<flatbuffers::Vector<float> *>(VT_VALUE);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_VALUE) &&
           verifier.VerifyVector(value()) &&
           verifier.EndTable();
  }
};

struct ArrayFloatBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_value(flatbuffers::Offset<flatbuffers::Vector<float>> value) {
    fbb_.AddOffset(ArrayFloat::VT_VALUE, value);
  }
  explicit ArrayFloatBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ArrayFloatBuilder &operator=(const ArrayFloatBuilder &);
  flatbuffers::Offset<ArrayFloat> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ArrayFloat>(end);
    return o;
  }
};

inline flatbuffers::Offset<ArrayFloat> CreateArrayFloat(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<float>> value = 0) {
  ArrayFloatBuilder builder_(_fbb);
  builder_.add_value(value);
  return builder_.Finish();
}

inline flatbuffers::Offset<ArrayFloat> CreateArrayFloatDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<float> *value = nullptr) {
  return CreateArrayFloat(
      _fbb,
      value ? _fbb.CreateVector<float>(*value) : 0);
}

struct DimensionMetaData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "DimensionMetaData";
  }
  enum {
    VT_LENGTH = 4,
    VT_UNIT = 6,
    VT_LABEL = 8,
    VT_BIN_BOUNDARIES_TYPE = 10,
    VT_BIN_BOUNDARIES = 12
  };
  uint32_t length() const {
    return GetField<uint32_t>(VT_LENGTH, 0);
  }
  bool mutate_length(uint32_t _length) {
    return SetField<uint32_t>(VT_LENGTH, _length, 0);
  }
  const flatbuffers::String *unit() const {
    return GetPointer<const flatbuffers::String *>(VT_UNIT);
  }
  flatbuffers::String *mutable_unit() {
    return GetPointer<flatbuffers::String *>(VT_UNIT);
  }
  const flatbuffers::String *label() const {
    return GetPointer<const flatbuffers::String *>(VT_LABEL);
  }
  flatbuffers::String *mutable_label() {
    return GetPointer<flatbuffers::String *>(VT_LABEL);
  }
  Array bin_boundaries_type() const {
    return static_cast<Array>(GetField<uint8_t>(VT_BIN_BOUNDARIES_TYPE, 0));
  }
  bool mutate_bin_boundaries_type(Array _bin_boundaries_type) {
    return SetField<uint8_t>(VT_BIN_BOUNDARIES_TYPE, static_cast<uint8_t>(_bin_boundaries_type), 0);
  }
  const void *bin_boundaries() const {
    return GetPointer<const void *>(VT_BIN_BOUNDARIES);
  }
  template<typename T> const T *bin_boundaries_as() const;
  const ArrayUInt *bin_boundaries_as_ArrayUInt() const {
    return bin_boundaries_type() == Array::ArrayUInt ? static_cast<const ArrayUInt *>(bin_boundaries()) : nullptr;
  }
  const ArrayULong *bin_boundaries_as_ArrayULong() const {
    return bin_boundaries_type() == Array::ArrayULong ? static_cast<const ArrayULong *>(bin_boundaries()) : nullptr;
  }
  const ArrayDouble *bin_boundaries_as_ArrayDouble() const {
    return bin_boundaries_type() == Array::ArrayDouble ? static_cast<const ArrayDouble *>(bin_boundaries()) : nullptr;
  }
  const ArrayFloat *bin_boundaries_as_ArrayFloat() const {
    return bin_boundaries_type() == Array::ArrayFloat ? static_cast<const ArrayFloat *>(bin_boundaries()) : nullptr;
  }
  void *mutable_bin_boundaries() {
    return GetPointer<void *>(VT_BIN_BOUNDARIES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT_LENGTH) &&
           VerifyOffset(verifier, VT_UNIT) &&
           verifier.VerifyString(unit()) &&
           VerifyOffset(verifier, VT_LABEL) &&
           verifier.VerifyString(label()) &&
           VerifyField<uint8_t>(verifier, VT_BIN_BOUNDARIES_TYPE) &&
           VerifyOffset(verifier, VT_BIN_BOUNDARIES) &&
           VerifyArray(verifier, bin_boundaries(), bin_boundaries_type()) &&
           verifier.EndTable();
  }
};

template<> inline const ArrayUInt *DimensionMetaData::bin_boundaries_as<ArrayUInt>() const {
  return bin_boundaries_as_ArrayUInt();
}

template<> inline const ArrayULong *DimensionMetaData::bin_boundaries_as<ArrayULong>() const {
  return bin_boundaries_as_ArrayULong();
}

template<> inline const ArrayDouble *DimensionMetaData::bin_boundaries_as<ArrayDouble>() const {
  return bin_boundaries_as_ArrayDouble();
}

template<> inline const ArrayFloat *DimensionMetaData::bin_boundaries_as<ArrayFloat>() const {
  return bin_boundaries_as_ArrayFloat();
}

struct DimensionMetaDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_length(uint32_t length) {
    fbb_.AddElement<uint32_t>(DimensionMetaData::VT_LENGTH, length, 0);
  }
  void add_unit(flatbuffers::Offset<flatbuffers::String> unit) {
    fbb_.AddOffset(DimensionMetaData::VT_UNIT, unit);
  }
  void add_label(flatbuffers::Offset<flatbuffers::String> label) {
    fbb_.AddOffset(DimensionMetaData::VT_LABEL, label);
  }
  void add_bin_boundaries_type(Array bin_boundaries_type) {
    fbb_.AddElement<uint8_t>(DimensionMetaData::VT_BIN_BOUNDARIES_TYPE, static_cast<uint8_t>(bin_boundaries_type), 0);
  }
  void add_bin_boundaries(flatbuffers::Offset<void> bin_boundaries) {
    fbb_.AddOffset(DimensionMetaData::VT_BIN_BOUNDARIES, bin_boundaries);
  }
  explicit DimensionMetaDataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  DimensionMetaDataBuilder &operator=(const DimensionMetaDataBuilder &);
  flatbuffers::Offset<DimensionMetaData> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<DimensionMetaData>(end);
    return o;
  }
};

inline flatbuffers::Offset<DimensionMetaData> CreateDimensionMetaData(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t length = 0,
    flatbuffers::Offset<flatbuffers::String> unit = 0,
    flatbuffers::Offset<flatbuffers::String> label = 0,
    Array bin_boundaries_type = Array::NONE,
    flatbuffers::Offset<void> bin_boundaries = 0) {
  DimensionMetaDataBuilder builder_(_fbb);
  builder_.add_bin_boundaries(bin_boundaries);
  builder_.add_label(label);
  builder_.add_unit(unit);
  builder_.add_length(length);
  builder_.add_bin_boundaries_type(bin_boundaries_type);
  return builder_.Finish();
}

inline flatbuffers::Offset<DimensionMetaData> CreateDimensionMetaDataDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t length = 0,
    const char *unit = nullptr,
    const char *label = nullptr,
    Array bin_boundaries_type = Array::NONE,
    flatbuffers::Offset<void> bin_boundaries = 0) {
  return CreateDimensionMetaData(
      _fbb,
      length,
      unit ? _fbb.CreateString(unit) : 0,
      label ? _fbb.CreateString(label) : 0,
      bin_boundaries_type,
      bin_boundaries);
}

struct EventHistogram FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "EventHistogram";
  }
  enum {
    VT_SOURCE = 4,
    VT_TIMESTAMP = 6,
    VT_DIM_METADATA = 8,
    VT_LAST_METADATA_TIMESTAMP = 10,
    VT_CURRENT_SHAPE = 12,
    VT_OFFSET = 14,
    VT_DATA_TYPE = 16,
    VT_DATA = 18,
    VT_ERRORS_TYPE = 20,
    VT_ERRORS = 22,
    VT_INFO = 24
  };
  const flatbuffers::String *source() const {
    return GetPointer<const flatbuffers::String *>(VT_SOURCE);
  }
  flatbuffers::String *mutable_source() {
    return GetPointer<flatbuffers::String *>(VT_SOURCE);
  }
  uint64_t timestamp() const {
    return GetField<uint64_t>(VT_TIMESTAMP, 0);
  }
  bool mutate_timestamp(uint64_t _timestamp) {
    return SetField<uint64_t>(VT_TIMESTAMP, _timestamp, 0);
  }
  const flatbuffers::Vector<flatbuffers::Offset<DimensionMetaData>> *dim_metadata() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<DimensionMetaData>> *>(VT_DIM_METADATA);
  }
  flatbuffers::Vector<flatbuffers::Offset<DimensionMetaData>> *mutable_dim_metadata() {
    return GetPointer<flatbuffers::Vector<flatbuffers::Offset<DimensionMetaData>> *>(VT_DIM_METADATA);
  }
  uint64_t last_metadata_timestamp() const {
    return GetField<uint64_t>(VT_LAST_METADATA_TIMESTAMP, 0);
  }
  bool mutate_last_metadata_timestamp(uint64_t _last_metadata_timestamp) {
    return SetField<uint64_t>(VT_LAST_METADATA_TIMESTAMP, _last_metadata_timestamp, 0);
  }
  const flatbuffers::Vector<uint32_t> *current_shape() const {
    return GetPointer<const flatbuffers::Vector<uint32_t> *>(VT_CURRENT_SHAPE);
  }
  flatbuffers::Vector<uint32_t> *mutable_current_shape() {
    return GetPointer<flatbuffers::Vector<uint32_t> *>(VT_CURRENT_SHAPE);
  }
  const flatbuffers::Vector<uint32_t> *offset() const {
    return GetPointer<const flatbuffers::Vector<uint32_t> *>(VT_OFFSET);
  }
  flatbuffers::Vector<uint32_t> *mutable_offset() {
    return GetPointer<flatbuffers::Vector<uint32_t> *>(VT_OFFSET);
  }
  Array data_type() const {
    return static_cast<Array>(GetField<uint8_t>(VT_DATA_TYPE, 0));
  }
  bool mutate_data_type(Array _data_type) {
    return SetField<uint8_t>(VT_DATA_TYPE, static_cast<uint8_t>(_data_type), 0);
  }
  const void *data() const {
    return GetPointer<const void *>(VT_DATA);
  }
  template<typename T> const T *data_as() const;
  const ArrayUInt *data_as_ArrayUInt() const {
    return data_type() == Array::ArrayUInt ? static_cast<const ArrayUInt *>(data()) : nullptr;
  }
  const ArrayULong *data_as_ArrayULong() const {
    return data_type() == Array::ArrayULong ? static_cast<const ArrayULong *>(data()) : nullptr;
  }
  const ArrayDouble *data_as_ArrayDouble() const {
    return data_type() == Array::ArrayDouble ? static_cast<const ArrayDouble *>(data()) : nullptr;
  }
  const ArrayFloat *data_as_ArrayFloat() const {
    return data_type() == Array::ArrayFloat ? static_cast<const ArrayFloat *>(data()) : nullptr;
  }
  void *mutable_data() {
    return GetPointer<void *>(VT_DATA);
  }
  Array errors_type() const {
    return static_cast<Array>(GetField<uint8_t>(VT_ERRORS_TYPE, 0));
  }
  bool mutate_errors_type(Array _errors_type) {
    return SetField<uint8_t>(VT_ERRORS_TYPE, static_cast<uint8_t>(_errors_type), 0);
  }
  const void *errors() const {
    return GetPointer<const void *>(VT_ERRORS);
  }
  template<typename T> const T *errors_as() const;
  const ArrayUInt *errors_as_ArrayUInt() const {
    return errors_type() == Array::ArrayUInt ? static_cast<const ArrayUInt *>(errors()) : nullptr;
  }
  const ArrayULong *errors_as_ArrayULong() const {
    return errors_type() == Array::ArrayULong ? static_cast<const ArrayULong *>(errors()) : nullptr;
  }
  const ArrayDouble *errors_as_ArrayDouble() const {
    return errors_type() == Array::ArrayDouble ? static_cast<const ArrayDouble *>(errors()) : nullptr;
  }
  const ArrayFloat *errors_as_ArrayFloat() const {
    return errors_type() == Array::ArrayFloat ? static_cast<const ArrayFloat *>(errors()) : nullptr;
  }
  void *mutable_errors() {
    return GetPointer<void *>(VT_ERRORS);
  }
  const flatbuffers::String *info() const {
    return GetPointer<const flatbuffers::String *>(VT_INFO);
  }
  flatbuffers::String *mutable_info() {
    return GetPointer<flatbuffers::String *>(VT_INFO);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_SOURCE) &&
           verifier.VerifyString(source()) &&
           VerifyField<uint64_t>(verifier, VT_TIMESTAMP) &&
           VerifyOffset(verifier, VT_DIM_METADATA) &&
           verifier.VerifyVector(dim_metadata()) &&
           verifier.VerifyVectorOfTables(dim_metadata()) &&
           VerifyField<uint64_t>(verifier, VT_LAST_METADATA_TIMESTAMP) &&
           VerifyOffsetRequired(verifier, VT_CURRENT_SHAPE) &&
           verifier.VerifyVector(current_shape()) &&
           VerifyOffset(verifier, VT_OFFSET) &&
           verifier.VerifyVector(offset()) &&
           VerifyField<uint8_t>(verifier, VT_DATA_TYPE) &&
           VerifyOffset(verifier, VT_DATA) &&
           VerifyArray(verifier, data(), data_type()) &&
           VerifyField<uint8_t>(verifier, VT_ERRORS_TYPE) &&
           VerifyOffset(verifier, VT_ERRORS) &&
           VerifyArray(verifier, errors(), errors_type()) &&
           VerifyOffset(verifier, VT_INFO) &&
           verifier.VerifyString(info()) &&
           verifier.EndTable();
  }
};

template<> inline const ArrayUInt *EventHistogram::data_as<ArrayUInt>() const {
  return data_as_ArrayUInt();
}

template<> inline const ArrayULong *EventHistogram::data_as<ArrayULong>() const {
  return data_as_ArrayULong();
}

template<> inline const ArrayDouble *EventHistogram::data_as<ArrayDouble>() const {
  return data_as_ArrayDouble();
}

template<> inline const ArrayFloat *EventHistogram::data_as<ArrayFloat>() const {
  return data_as_ArrayFloat();
}

template<> inline const ArrayUInt *EventHistogram::errors_as<ArrayUInt>() const {
  return errors_as_ArrayUInt();
}

template<> inline const ArrayULong *EventHistogram::errors_as<ArrayULong>() const {
  return errors_as_ArrayULong();
}

template<> inline const ArrayDouble *EventHistogram::errors_as<ArrayDouble>() const {
  return errors_as_ArrayDouble();
}

template<> inline const ArrayFloat *EventHistogram::errors_as<ArrayFloat>() const {
  return errors_as_ArrayFloat();
}

struct EventHistogramBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_source(flatbuffers::Offset<flatbuffers::String> source) {
    fbb_.AddOffset(EventHistogram::VT_SOURCE, source);
  }
  void add_timestamp(uint64_t timestamp) {
    fbb_.AddElement<uint64_t>(EventHistogram::VT_TIMESTAMP, timestamp, 0);
  }
  void add_dim_metadata(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<DimensionMetaData>>> dim_metadata) {
    fbb_.AddOffset(EventHistogram::VT_DIM_METADATA, dim_metadata);
  }
  void add_last_metadata_timestamp(uint64_t last_metadata_timestamp) {
    fbb_.AddElement<uint64_t>(EventHistogram::VT_LAST_METADATA_TIMESTAMP, last_metadata_timestamp, 0);
  }
  void add_current_shape(flatbuffers::Offset<flatbuffers::Vector<uint32_t>> current_shape) {
    fbb_.AddOffset(EventHistogram::VT_CURRENT_SHAPE, current_shape);
  }
  void add_offset(flatbuffers::Offset<flatbuffers::Vector<uint32_t>> offset) {
    fbb_.AddOffset(EventHistogram::VT_OFFSET, offset);
  }
  void add_data_type(Array data_type) {
    fbb_.AddElement<uint8_t>(EventHistogram::VT_DATA_TYPE, static_cast<uint8_t>(data_type), 0);
  }
  void add_data(flatbuffers::Offset<void> data) {
    fbb_.AddOffset(EventHistogram::VT_DATA, data);
  }
  void add_errors_type(Array errors_type) {
    fbb_.AddElement<uint8_t>(EventHistogram::VT_ERRORS_TYPE, static_cast<uint8_t>(errors_type), 0);
  }
  void add_errors(flatbuffers::Offset<void> errors) {
    fbb_.AddOffset(EventHistogram::VT_ERRORS, errors);
  }
  void add_info(flatbuffers::Offset<flatbuffers::String> info) {
    fbb_.AddOffset(EventHistogram::VT_INFO, info);
  }
  explicit EventHistogramBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  EventHistogramBuilder &operator=(const EventHistogramBuilder &);
  flatbuffers::Offset<EventHistogram> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<EventHistogram>(end);
    fbb_.Required(o, EventHistogram::VT_CURRENT_SHAPE);
    return o;
  }
};

inline flatbuffers::Offset<EventHistogram> CreateEventHistogram(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> source = 0,
    uint64_t timestamp = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<DimensionMetaData>>> dim_metadata = 0,
    uint64_t last_metadata_timestamp = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint32_t>> current_shape = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint32_t>> offset = 0,
    Array data_type = Array::NONE,
    flatbuffers::Offset<void> data = 0,
    Array errors_type = Array::NONE,
    flatbuffers::Offset<void> errors = 0,
    flatbuffers::Offset<flatbuffers::String> info = 0) {
  EventHistogramBuilder builder_(_fbb);
  builder_.add_last_metadata_timestamp(last_metadata_timestamp);
  builder_.add_timestamp(timestamp);
  builder_.add_info(info);
  builder_.add_errors(errors);
  builder_.add_data(data);
  builder_.add_offset(offset);
  builder_.add_current_shape(current_shape);
  builder_.add_dim_metadata(dim_metadata);
  builder_.add_source(source);
  builder_.add_errors_type(errors_type);
  builder_.add_data_type(data_type);
  return builder_.Finish();
}

inline flatbuffers::Offset<EventHistogram> CreateEventHistogramDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *source = nullptr,
    uint64_t timestamp = 0,
    const std::vector<flatbuffers::Offset<DimensionMetaData>> *dim_metadata = nullptr,
    uint64_t last_metadata_timestamp = 0,
    const std::vector<uint32_t> *current_shape = nullptr,
    const std::vector<uint32_t> *offset = nullptr,
    Array data_type = Array::NONE,
    flatbuffers::Offset<void> data = 0,
    Array errors_type = Array::NONE,
    flatbuffers::Offset<void> errors = 0,
    const char *info = nullptr) {
  return CreateEventHistogram(
      _fbb,
      source ? _fbb.CreateString(source) : 0,
      timestamp,
      dim_metadata ? _fbb.CreateVector<flatbuffers::Offset<DimensionMetaData>>(*dim_metadata) : 0,
      last_metadata_timestamp,
      current_shape ? _fbb.CreateVector<uint32_t>(*current_shape) : 0,
      offset ? _fbb.CreateVector<uint32_t>(*offset) : 0,
      data_type,
      data,
      errors_type,
      errors,
      info ? _fbb.CreateString(info) : 0);
}

inline bool VerifyArray(flatbuffers::Verifier &verifier, const void *obj, Array type) {
  switch (type) {
    case Array::NONE: {
      return true;
    }
    case Array::ArrayUInt: {
      auto ptr = reinterpret_cast<const ArrayUInt *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Array::ArrayULong: {
      auto ptr = reinterpret_cast<const ArrayULong *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Array::ArrayDouble: {
      auto ptr = reinterpret_cast<const ArrayDouble *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Array::ArrayFloat: {
      auto ptr = reinterpret_cast<const ArrayFloat *>(obj);
      return verifier.VerifyTable(ptr);
    }
    default: return false;
  }
}

inline bool VerifyArrayVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types) {
  if (!values || !types) return !values && !types;
  if (values->size() != types->size()) return false;
  for (flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
    if (!VerifyArray(
        verifier,  values->Get(i), types->GetEnum<Array>(i))) {
      return false;
    }
  }
  return true;
}

inline const EventHistogram *GetEventHistogram(const void *buf) {
  return flatbuffers::GetRoot<EventHistogram>(buf);
}

inline const EventHistogram *GetSizePrefixedEventHistogram(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<EventHistogram>(buf);
}

inline EventHistogram *GetMutableEventHistogram(void *buf) {
  return flatbuffers::GetMutableRoot<EventHistogram>(buf);
}

inline const char *EventHistogramIdentifier() {
  return "hs00";
}

inline bool EventHistogramBufferHasIdentifier(const void *buf) {
  return flatbuffers::BufferHasIdentifier(
      buf, EventHistogramIdentifier());
}

inline bool VerifyEventHistogramBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<EventHistogram>(EventHistogramIdentifier());
}

inline bool VerifySizePrefixedEventHistogramBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<EventHistogram>(EventHistogramIdentifier());
}

inline void FinishEventHistogramBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<EventHistogram> root) {
  fbb.Finish(root, EventHistogramIdentifier());
}

inline void FinishSizePrefixedEventHistogramBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<EventHistogram> root) {
  fbb.FinishSizePrefixed(root, EventHistogramIdentifier());
}
}

#include "pch.h"
#include "MaterialProperty.h"

namespace URM::Core {
	void MaterialProperty::ThrowIfNotValidType(Types expectedType) const {
		if (type != expectedType) {
			throw std::runtime_error("Invalid type");
		}
	}

	MaterialProperty::MaterialProperty(const std::string& name, const std::string& data) {
		this->name = name;
		this->type = Types::STRING;
		this->mDataLength = data.length();
		this->mBufferSize = data.length();
		new(&this->stringData) std::string(data);
	}

	void MaterialProperty::CopyFrom(const MaterialProperty& other) {
		this->name = other.name;
		this->type = other.type;
		this->mDataLength = other.mDataLength;
		this->mBufferSize = other.mBufferSize;
		if (type == Types::STRING) {
			new(&this->stringData) std::string(other.stringData);
		}
		else {
			rawData = new BYTE[other.mBufferSize];
			memcpy(rawData, other.rawData, other.mBufferSize);
		}
	}

	MaterialProperty::MaterialProperty(const std::string& name, Types type, const BYTE* data, size_t bufferSize, size_t dataLength) {
		this->name = name;
		this->type = type;
		this->mDataLength = dataLength;
		this->mBufferSize = bufferSize;
		if (type == Types::STRING) {
			throw std::runtime_error("MaterialProperty array type constructor used for String data.");
		}

		rawData = new BYTE[bufferSize];
		memcpy(rawData, data, bufferSize);
	}

	MaterialProperty::MaterialProperty(const MaterialProperty& other) {
		this->CopyFrom(other);
	}

	MaterialProperty MaterialProperty::operator=(const MaterialProperty& other) const {
		auto prop = MaterialProperty(other.name, other.type, other.rawData, other.mBufferSize, other.mDataLength);
		return prop;
	}

	MaterialProperty::~MaterialProperty() {
		if (type == Types::STRING) {
			this->stringData.~basic_string();
		}
		else {
			delete[] rawData;
		}
	}

	MaterialProperty::PropertyValue<float> MaterialProperty::GetFloatArray() const {
		ThrowIfNotValidType(Types::FLOAT);
		return PropertyValue<float>(rawData, mDataLength);
	}

	MaterialProperty::PropertyValue<double> MaterialProperty::GetDoubleArray() const {
		ThrowIfNotValidType(Types::DOUBLE);
		return PropertyValue<double>(rawData, mDataLength);
	}

	std::string MaterialProperty::GetString() {
		ThrowIfNotValidType(Types::STRING);
		return stringData;
	}

	MaterialProperty::PropertyValue<int> MaterialProperty::GetIntegerArray() const {
		ThrowIfNotValidType(Types::INTEGER);
		return PropertyValue<int>(rawData, mDataLength);
	}

	MaterialProperty::PropertyValue<void> MaterialProperty::GetBuffer() const {
		ThrowIfNotValidType(Types::BUFFER);
		return PropertyValue<void>(rawData, mDataLength);
	}

	template<typename T>
	std::string ValueArrayToString(MaterialProperty::PropertyValue<T> values) {
		std::string result = "[";
		for (size_t i = 0; i < values.length; i++) {
			result += std::to_string(values.data[i]);
			if (i < values.length - 1) {
				result += ", ";
			}
		}
		return result + "]";
	}

	std::string MaterialProperty::GetValueAsString() {
		switch (type) {
			case Types::STRING:
				return stringData;

			case Types::FLOAT:
				return ValueArrayToString(GetFloatArray());

			case Types::DOUBLE:
				return ValueArrayToString(GetDoubleArray());

			case Types::INTEGER:
				return ValueArrayToString(GetIntegerArray());

			case Types::BUFFER: {
				std::string result = "{";
				for (size_t i = 0; i < mDataLength; i++) {
					result += std::to_string(rawData[i]);
					if (i < mDataLength - 1) {
						result += ", ";
					}
				}
				return result + "}";
			}

			default:
				throw std::runtime_error("Unsupported type for string conversion");
		}
	}

	MaterialProperty MaterialProperty::CreateFloat(const std::string& name, std::vector<float> values) {
		return MaterialProperty(name, Types::FLOAT, reinterpret_cast<BYTE*>(values.data()), values.size() * 4, values.size());
	}

	MaterialProperty MaterialProperty::CreateDouble(const std::string& name, std::vector<double> values) {
		return MaterialProperty(name, Types::DOUBLE, reinterpret_cast<BYTE*>(values.data()), values.size() * 8, values.size());
	}

	MaterialProperty MaterialProperty::CreateString(const std::string& name, const char* value, size_t length) {
		return MaterialProperty(name, std::string(value, value + length));
	}

	MaterialProperty MaterialProperty::CreateInteger(const std::string& name, std::vector<int> values) {
		return MaterialProperty(name, Types::INTEGER, reinterpret_cast<BYTE*>(values.data()), values.size() * 4, values.size());
	}

	MaterialProperty MaterialProperty::CreateBuffer(const std::string& name, void* buffer, size_t dataLength) {
		return MaterialProperty(name, Types::BUFFER, static_cast<BYTE*>(buffer), dataLength, dataLength);
	}
}

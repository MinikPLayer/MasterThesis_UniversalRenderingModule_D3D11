#include "pch.h"
#include "MaterialProperty.h"

namespace URM::Core {
	void MaterialProperty::ThrowIfNotValidType(Types expectedType) const {
		if (type != expectedType) {
			throw std::runtime_error("Invalid type");
		}
	}

	MaterialProperty::MaterialProperty(std::string name, std::string data) {
		this->name = name;
		this->type = Types::STRING;
		this->dataLength = data.length();
		new(&this->stringData) std::string(data);
	}

	void MaterialProperty::CopyFrom(const MaterialProperty& other) {
		this->name = other.name;
		this->type = other.type;
		this->dataLength = other.dataLength;
		this->bufferSize = other.bufferSize;
		if (type == Types::STRING) {
			new(&this->stringData) std::string(other.stringData);
		}
		else {
			rawData = new BYTE[other.bufferSize];
			memcpy(rawData, other.rawData, other.bufferSize);
		}
	}

	MaterialProperty::MaterialProperty(std::string name, Types type, BYTE* data, size_t bufferSize, size_t dataLength) {
		this->name = name;
		this->type = type;
		this->dataLength = dataLength;
		this->bufferSize = bufferSize;
		if (type == Types::STRING) {
			throw std::runtime_error("MaterialProperty array type constructor used for String data.");
		}

		rawData = new BYTE[bufferSize];
		memcpy(rawData, data, bufferSize);
	}

	MaterialProperty::MaterialProperty(const MaterialProperty& other) {
		this->CopyFrom(other);
	}

	MaterialProperty MaterialProperty::operator=(const MaterialProperty& other) {
		auto prop = MaterialProperty(other.name, other.type, other.rawData, other.bufferSize, other.dataLength);
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

	MaterialProperty::PropertyValue<float> MaterialProperty::GetFloatArray()
	{
		ThrowIfNotValidType(Types::FLOAT);
		return PropertyValue<float>(rawData, dataLength);
	}

	MaterialProperty::PropertyValue<double> MaterialProperty::GetDoubleArray()
	{
		ThrowIfNotValidType(Types::DOUBLE);
		return PropertyValue<double>(rawData, dataLength);
	}

	std::string MaterialProperty::GetString()
	{
		ThrowIfNotValidType(Types::STRING);
		return stringData;
	}

	MaterialProperty::PropertyValue<int> MaterialProperty::GetIntegerArray()
	{
		ThrowIfNotValidType(Types::INTEGER);
		return PropertyValue<int>(rawData, dataLength);
	}

	MaterialProperty::PropertyValue<void> MaterialProperty::GetBuffer()
	{
		ThrowIfNotValidType(Types::BUFFER);
		return PropertyValue<void>(rawData, dataLength);
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

	std::string MaterialProperty::GetValueAsString()
	{
		switch (type) {
		case Types::STRING:
			return stringData;

		case Types::FLOAT:
			return ValueArrayToString(GetFloatArray());

		case Types::DOUBLE:
			return ValueArrayToString(GetDoubleArray());

		case Types::INTEGER:
			return ValueArrayToString(GetIntegerArray());

		case Types::BUFFER:
		{
			std::string result = "{";
			for (size_t i = 0; i < dataLength; i++) {
				result += std::to_string(((BYTE*)rawData)[i]);
				if (i < dataLength - 1) {
					result += ", ";
				}
			}
			return result + "}";
		}

		default:
			throw std::runtime_error("Unsupported type for string conversion");
		}
	}

	MaterialProperty MaterialProperty::CreateFloat(std::string name, std::vector<float> values) {
		return MaterialProperty(name, Types::FLOAT, (BYTE*)values.data(), values.size() * 4, values.size());
	}

	MaterialProperty MaterialProperty::CreateDouble(std::string name, std::vector<double> values) {
		return MaterialProperty(name, Types::DOUBLE, (BYTE*)values.data(), values.size() * 8, values.size());
	}

	MaterialProperty MaterialProperty::CreateString(std::string name, const char* value, size_t length) {
		return MaterialProperty(name, std::string(value, value + length));
	}

	MaterialProperty MaterialProperty::CreateInteger(std::string name, std::vector<int> values) {
		return MaterialProperty(name, Types::INTEGER, (BYTE*)values.data(), values.size() * 4, values.size());
	}

	MaterialProperty MaterialProperty::CreateBuffer(std::string name, void* buffer, size_t dataLength) {
		return MaterialProperty(name, Types::BUFFER, (BYTE*)buffer, dataLength, dataLength);
	}
}
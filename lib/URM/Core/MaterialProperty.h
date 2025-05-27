#pragma once

#include <string>
#include <stdexcept>

namespace URM::Core {
	class MaterialProperty {
		union {
			BYTE* rawData = nullptr;
			std::string stringData;
		};

		size_t dataLength;
		size_t bufferSize;
	public:
		enum class Types {
			FLOAT,
			DOUBLE,
			STRING,
			INTEGER,
			BUFFER
		};

		template<typename T>
		struct PropertyValue {
			T* data;
			size_t length;

			PropertyValue(void* data, size_t size) : data((T*)data), length(size) {}
		};

		std::string name;
		MaterialProperty::Types type;

	private:
		void ThrowIfNotValidType(Types expectedType) const;

		MaterialProperty(std::string name, Types type, BYTE* data, size_t bufferSize, size_t dataLength);
		MaterialProperty(std::string name, std::string data);

		void CopyFrom(const MaterialProperty& other);

	public:
		MaterialProperty(const MaterialProperty& other);
		MaterialProperty operator=(const MaterialProperty& other);
		~MaterialProperty();

		PropertyValue<float> GetFloatArray();
		PropertyValue<double> GetDoubleArray();
		PropertyValue<int> GetIntegerArray();
		std::string GetString();
		PropertyValue<void> GetBuffer();

		std::string GetValueAsString();

		static MaterialProperty CreateFloat(std::string name, std::vector<float> values);
		static MaterialProperty CreateDouble(std::string name, std::vector<double> values);
		static MaterialProperty CreateInteger(std::string name, std::vector<int> values);
		static MaterialProperty CreateString(std::string name, const char* value, size_t length);
		static MaterialProperty CreateBuffer(std::string name, void* buffer, size_t dataLength);
	};
}
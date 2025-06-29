#pragma once

#include <string>
#include <stdexcept>
#include <variant>

namespace URM::Core {
	class MaterialProperty {
		std::variant<BYTE*, std::string> data;

		size_t mDataLength;
		size_t mBufferSize;
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

			PropertyValue(void* data, size_t size) : data(static_cast<T*>(data)), length(size) {}
		};

		std::string name;
	private:
		Types type;

		void ThrowIfNotValidType(Types expectedType) const;

		MaterialProperty(const std::string& name, Types type, const BYTE* data, size_t bufferSize, size_t dataLength);
		MaterialProperty(const std::string& name, const std::string& data);

		void CopyFrom(const MaterialProperty& other);
	public:
		Types GetType() const {
			return this->type;
		}

		MaterialProperty(const MaterialProperty& other);
		MaterialProperty operator=(const MaterialProperty& other) const;
		~MaterialProperty();

		PropertyValue<float> GetFloatArray() const;
		PropertyValue<double> GetDoubleArray() const;
		PropertyValue<int> GetIntegerArray() const;
		std::string GetString();
		PropertyValue<void> GetBuffer() const;

		std::string GetValueAsString();

		static MaterialProperty CreateFloat(const std::string& name, std::vector<float> values);
		static MaterialProperty CreateDouble(const std::string& name, std::vector<double> values);
		static MaterialProperty CreateInteger(const std::string& name, std::vector<int> values);
		static MaterialProperty CreateString(const std::string& name, const char* value, size_t length);
		static MaterialProperty CreateBuffer(const std::string& name, void* buffer, size_t dataLength);
	};
}

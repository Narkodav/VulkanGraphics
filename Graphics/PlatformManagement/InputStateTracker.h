#pragma once
#include <array>
#include <type_traits>
#include <limits>

template <typename InputEnum, size_t NUM_INPUTS,
	typename InputStateEnum, size_t NUM_INPUT_STATES,
	typename StorageType = uint8_t>
	requires std::is_enum_v<InputEnum>&&
std::is_enum_v<InputStateEnum>&&
std::is_scalar_v<StorageType>&&
std::is_unsigned_v<StorageType>
class InputStateTracker
{
private:
	//static_assert(NUM_INPUTS > 0, "Number of inputs must be greater than 0");
	//static_assert(NUM_INPUT_STATES > 0, "Number of input states must be greater than 0");
	//static_assert(sizeof(StorageType) / 8 < std::numeric_limits<size_t>::max(),
	//	"StorageType size in bits cannot exceed maximum value of size_t");
	//static_assert(NUM_INPUTS <= std::numeric_limits<size_t>::max() / 8, "NUM_INPUTS too large");
	//static_assert(NUM_INPUT_STATES <= std::numeric_limits<size_t>::max(), "NUM_INPUT_STATES too large");

	static inline const size_t STORAGE_SIZE = sizeof(StorageType) * 8;

	using StateArray = std::array<StorageType, (NUM_INPUTS - 1) / STORAGE_SIZE + 1>;
	std::array<StateArray, NUM_INPUT_STATES> m_stateArrays = { 0 };

	static inline constexpr std::array<size_t, NUM_INPUTS> inputByteIndices = []() {
		std::array<size_t, NUM_INPUTS> InputIndices;
		for (int i = 0; i < NUM_INPUTS; ++i)
			InputIndices[i] = i / STORAGE_SIZE;
		return InputIndices;
		}();

	static inline constexpr std::array<size_t, NUM_INPUTS> inputBitOffsets = []() {
		std::array<size_t, NUM_INPUTS> InputOffsets;
		for (int i = 0; i < NUM_INPUTS; ++i)
			InputOffsets[i] = i % STORAGE_SIZE;
		return InputOffsets;
		}();

protected:

	template<InputStateEnum S_E>
	void setInputState(InputEnum Input, bool state)
	{
		auto mask = static_cast<StorageType>(1) << inputBitOffsets[static_cast<size_t>(Input)];
		auto stateMask = static_cast<StorageType>(state) << inputBitOffsets[static_cast<size_t>(Input)];
		m_stateArrays[static_cast<size_t>(S_E)]
			[inputByteIndices[static_cast<size_t>(Input)]] =
			stateMask | (~mask & m_stateArrays[static_cast<size_t>(S_E)]
				[inputByteIndices[static_cast<size_t>(Input)]]);
	}

	template<InputStateEnum S_E>
	void toggleInputState(InputEnum Input)
	{
		m_stateArrays[static_cast<size_t>(S_E)]
			[inputByteIndices[static_cast<size_t>(Input)]] ^=
			static_cast<StorageType>(1) << inputBitOffsets[static_cast<size_t>(Input)];
	}

public:
	template<InputEnum E, InputStateEnum S_E>
	bool getInputState() const
	{
		return m_stateArrays[static_cast<size_t>(S_E)]
			[inputByteIndices[static_cast<size_t>(E)]] >> inputBitOffsets[static_cast<size_t>(E)] & 1;
	}

	template<InputStateEnum S_E>
	bool getInputState(InputEnum Input) const
	{
		return m_stateArrays[static_cast<size_t>(S_E)]
			[inputByteIndices[static_cast<size_t>(Input)]] >> inputBitOffsets[static_cast<size_t>(Input)] & 1;
	}
};

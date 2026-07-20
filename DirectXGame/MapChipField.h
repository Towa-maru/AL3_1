#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
enum class MapChipType {
	kBlank,
	kBlock,
};
struct IndexSet {

	uint32_t xIndex;
	uint32_t yIndex;
};
class MapChipField {
public:
	struct MapChipData {
		std::vector<std::vector<MapChipType>> data;
	};
	struct Rect {

		float left;
		float right;
		float bottom;
		float top;
	};
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetmapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetmapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	uint32_t GetNumBlockVirtical() const;
	uint32_t GetNumBlockHorizontal() const;

	MapChipField();

private:
	MapChipData mapChipData_;

	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
};
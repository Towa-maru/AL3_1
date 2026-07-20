#include "MapChipField.h"

#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
using namespace KamataEngine;
namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};

}

void MapChipField::ResetMapChipData() {

	mapChipData_.data.clear();

	mapChipData_.data.resize(kNumBlockVirtical);

	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);

	assert(file.is_open());

	std::stringstream loadMapChipCsv;
	loadMapChipCsv << file.rdbuf();

	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {

		std::string line;
		getline(loadMapChipCsv, line);

		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {

			std::string word;
			std::getline(lineStream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetmapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {

	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {

	IndexSet indexSet{};

	int x = static_cast<int>(std::floor(position.x / kBlockWidth));

	int y = static_cast<int>(std::floor((kBlockHeight * kNumBlockVirtical - position.y) / kBlockHeight));

	// clamp safely
	x = (std::max)(0, (std::min)(x, static_cast<int>(kNumBlockHorizontal) - 1));
	y = (std::max)(0, (std::min)(y, static_cast<int>(kNumBlockVirtical) - 1));

	indexSet.xIndex = static_cast<uint32_t>(x);

	indexSet.yIndex = static_cast<uint32_t>(y);

	return indexSet;
}
uint32_t MapChipField::GetNumBlockVirtical() const { return kNumBlockVirtical; }

uint32_t MapChipField::GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

MapChipField::MapChipField() {}

Vector3 MapChipField::GetmapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {

	return Vector3(xIndex * kBlockWidth + kBlockWidth / 2.0f, (kNumBlockVirtical - yIndex - 1) * kBlockHeight + kBlockHeight / 2.0f, 5.0f);
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	Vector3 center = GetmapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;    // fixed: was kBlockHeight
	rect.right = center.x + kBlockWidth / 2.0f;   // fixed: was kBlockHeight
	rect.bottom = center.y - kBlockHeight / 2.0f; // fixed: was kBlockWidth
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;
}
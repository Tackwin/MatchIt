#pragma once

struct Item {
	static const int LEFT = -1;
	static const int RIGHT = 1;
	static const int EITHER = 0;
	static const int WAINTINGMAX = 5;

	int target = 0;
	int r = 0;

	int direction = 0;
	int waitingPos = 0;
};
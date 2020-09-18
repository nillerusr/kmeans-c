//
// Created by nillerusr on 06.02.20.
//

#ifndef COLOR_ANALYZER_DB_H
#define COLOR_ANALYZER_DB_H

#include "stdbool.h"
#include "images.h"

bool openColorsDatabase();
point_t getColorFromDatabase(const char *word);
void addColorToDatabase(const char *word, int r, int g, int b);

#endif //COLOR_ANALYZER_DB_H
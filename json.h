#ifndef JSON_H
#define JSON_H

/**
 * @brief Run simulation using external commands and print output to JSON
 * @param *inPath Input data file (binary) path
 * @param *outPath Output JSON file
 * @return 0 on success, <0 on failure
 */
int JsonRunSimFromExternalData(const char *inPath, const char *outPath);

#endif
#include "../SnapTime.hpp"

const char* const SchemaOnePath = "tests/test_schemas/schema_1.schema";
const char* const ScratchDirectory = "tests/Scratch/";
const char* const ScratchDirPath = "tests/Scratch/ScratchOutput/";
const char* const ScratchInputData = "tests/test_data/";
const char* const ScratchSymbolicDir = "tests/Scratch/SymDirOutput/";
const char* const ScratchQueryOutput = "tests/Scratch/QueryOutput.bin";

void CleanTestDirectory() {
	if(TDir::Exists(ScratchDirectory)) {
		AssertR(TDir::DelDir(ScratchDirectory), "could not delete scratch directory");
	}
	AssertR(TDir::GenDir(ScratchDirectory), "could not create scratch directory");
}
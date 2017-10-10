#include "../SnapTime.hpp"

const char* const SchemaOnePath = "tests/test_schemas/schema_1.schema";
const char* const ScratchDirectory = "tests/Scratch/";
const char* const ScratchDirPath = "tests/Scratch/ScratchOutput/";
const char* const InputData = "tests/test_data/";
const char* const ScratchSymbolicDir = "tests/Scratch/SymDirOutput/";
const char* const ScratchQueryOutput = "tests/Scratch/QueryOutput.bin";

void CreateTestDirectory() {
	if (!TDir::Exists(ScratchDirectory)) {
		AssertR(TDir::GenDir(ScratchDirectory), "could not create scratch directory");
	}
}
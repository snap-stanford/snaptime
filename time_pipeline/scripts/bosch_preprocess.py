import sys
import os

def main(dir_name):
	dirs = os.listdir(path)
	for file in dirs:
		print os.path.join(root, file)

if __name__ == "__main__":
	assert len(sys.argv) == 2
    main(sys.argv[1])

    
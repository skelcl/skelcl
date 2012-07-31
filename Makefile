# search for all direct subfolders starting with the name 'build'
BUILD_DIRS := ${shell find . -maxdepth 1 -mindepth 1 -name 'build*' -type d -print}

.PHONY: all

# execute make in all subfolders
all:
	@for d in $(BUILD_DIRS); do \
		$(MAKE) -j4 -C $$d ; \
	done


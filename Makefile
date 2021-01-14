VPATH=utils:$(OUT_DIR)

OUT_DIR = build
MKDIR_P = mkdir -p
OBJECTS = yaml.o dependencies.o string_utils.o

all: directories yaml
.PHONY: all

.PHONY: directories
directories: ${OUT_DIR}
${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

yaml: $(OBJECTS)
	gcc -o ${OUT_DIR}/yaml $(OBJECTS)

.PHONY : clean
clean :
	-rm $(OBJECTS)
	-rm -rf $(OUT_DIR)

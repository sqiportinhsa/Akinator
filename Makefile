CPPFLAGS = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

AKINATOR = build/akinator.exe

FOLDERS = obj build

.PHONY: all

all: folders $(AKINATOR)

clean: 
	find . -name "*.o" -delete

folders:
	mkdir -p $(FOLDERS)

$(AKINATOR): obj/akinator.o obj/tree.o obj/file_reading.o obj/logging.o obj/stack.o obj/stack_logs.o obj/stack_verification.o obj/main.o
	g++ obj/main.o obj/akinator.o obj/tree.o obj/file_reading.o obj/logging.o obj/stack.o obj/stack_logs.o obj/stack_verification.o -o $(AKINATOR) $(CPPFLAGS)

obj/main.o: main.cpp obj/akinator.o obj/tree.o 
	g++ -c main.cpp -o obj/main.o

obj/akinator.o: akinator.cpp akinator.h Tree/tree.cpp Tree/tree.h
	g++ -c akinator.cpp -o obj/akinator.o $(CPPFLAGS)



obj/tree.o: Tree/tree.cpp Tree/tree.h
	g++ -c Tree/tree.cpp -o obj/tree.o $(CPPFLAGS)



obj/stack.o: Libs/Stack/stack.cpp Tree/tree.h
	g++ -c Libs/Stack/stack.cpp -o obj/stack.o $(CPPFLAGS)

obj/stack_logs.o: Libs/Stack/stack_logs.cpp
	g++ -c Libs/Stack/stack_logs.cpp -o obj/stack_logs.o $(CPPFLAGS)

obj/stack_verification.o: Libs/Stack/stack_verification.cpp
	g++ -c Libs/Stack/stack_verification.cpp -o obj/stack_verification.o $(CPPFLAGS)



obj/file_reading.o: Libs/file_reading.cpp Libs/file_reading.hpp
	g++ -c Libs/file_reading.cpp -o obj/file_reading.o $(CPPFLAGS)



obj/logging.o: Libs/logging.cpp Libs/logging.h
	g++ -c Libs/logging.cpp -o obj/logging.o
 
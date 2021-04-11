#TODO: replace ID with your own IDS, for example: 123456789_123456789
SUBMITTERS := 320907645_207286469
COMPILER := g++
COMPILER_FLAGS := --std=c++11 -Wall
SRCS := Commands.cpp signals.cpp smash.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
HDRS := Commands.h signals.h
SMASH_BIN := smash

$(SMASH_BIN): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

$(OBJS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $^

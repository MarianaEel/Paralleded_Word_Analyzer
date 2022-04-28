.SUFFIXES:
.SUFFIXES: .o .cpp
#============================================================

TARGET1 = searchstr_map
C_OBJS1 = searchstr_map.o

# TARGET2 = searchstr_map_ACC
# C_OBJS2 = searchstr_map_ACC.o



ALL_SOURCES = Makefile $(C_SOURCES) $(MY_INCLUDES)

CCX = g++
CXXFLAGS = -g -std=c++11
# -Wall

#============================================================
all: $(TARGET1) 
# $(TARGET2) 

.o:.cpp	$(MY_INCLUDES)
	$(CCX)  -c  $(CXXFLAGS) $<  

$(TARGET1) :   $(C_OBJS1)
	$(CCX) $(CXXFLAGS)  $^ $(LIBDIRS)  -o $@

# $(TARGET2) :   $(C_OBJS2)
# 	$(CCX) $(CXXFLAGS)  $^ $(LIBDIRS)  -o $@


# Implicit rules: $@ = target name, $< = first prerequisite name, $^ = name of all prerequisites
#============================================================

ALL_SOURCES = Makefile $(C_SOURCES) $(MY_INCLUDES)

NOTES =
%= otherstuff.np 

clean:
	rm -f $(TARGET) $(C_OBJS) core 

tar: $(ALL_SOURCES) $(NOTES)
	tar cvf $(TARGET).tar $(ALL_SOURCES)  $(NOTES)

$(TARGET).ps: $(ALL SOURCES)
	enscript -pcode.ps $(ALL_SOURCES)


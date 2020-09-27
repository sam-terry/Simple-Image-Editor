CXX = g++
CXXFLAGS = -std=c++11
TARGET = main
OBJECT = image
NAME = 'Image Editor'

$(TARGET): $(TARGET).cpp $(OBJECT).h $(OBJECT).cpp
	$(CXX) $(CXXFLAGS) -o $(NAME) $(TARGET).cpp $(OBJECT).cpp

clean:
	$(RM) $(TARGET)

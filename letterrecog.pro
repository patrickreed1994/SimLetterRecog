HEADERS += letterrecog.h
SOURCES += letterrecog.cpp
SOURCES += neural.cpp

target.path = letterrecog
sources.files= $$SOURCES $$HEADERS letterrecog.pro
sources.path = .
INSTALLS += target sources

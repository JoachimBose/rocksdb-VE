engineswap_SOURCES = engineswap.cc aioengine.cc posixengine.cc iouengine.cc filedummies.cc
engineswap_HEADERS = engineswap.h aioengine.h posixengine.h iouengine.h filedummies.h
engineswap_LDFLAGS = -lcrypto -u engineswap_reg

engineswap_SOURCES = engineswap.cc aioengine.cc posixengine.cc iouengine.cc filedummies.cc util.cc filealarms.cc
engineswap_HEADERS = engineswap.h aioengine.h posixengine.h iouengine.h filedummies.h util.h filealarms.h
engineswap_LDFLAGS = -lcrypto -u engineswap_reg

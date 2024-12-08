engineswap_SOURCES = engineswap.cc aioengine.cc posixengine.cc iouengine.cc filedummies.cc util.cc filealarms.cc iounvengine.cc io_util.cc
engineswap_HEADERS = engineswap.h aioengine.h posixengine.h iouengine.h filedummies.h util.h filealarms.h iounvengine.h io_util.h
engineswap_LDFLAGS = -lcrypto -laio -u engineswap_reg

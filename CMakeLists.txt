build_lib(
  LIBNAME blood-voyager-s
  
  SOURCE_FILES Bloodvessel.cc
    Bloodstream.cc
    Nanobot.cc
    Bloodcircuit.cc
    Orchestrator.cc
    BVSChannel.cc
    NanoNetDevice.cc
    GatewayNetDevice.cc
    terahertz.cc
    
	
   
  HEADER_FILES Bloodvessel.h
    Bloodstream.h
    Nanobot.h
    Bloodcircuit.h
    Orchestrator.h
    BVSChannel.h
    NanoNetDevice.h
    GatewayNetDevice.h
    terahertz.h
  
  LIBRARIES_TO_LINK
    ${libinternet}
    ${libcore}
    ${libmobility}
    ${libwifi}
    ${libnetwork}
)
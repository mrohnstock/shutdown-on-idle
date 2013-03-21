{
  "targets": [
    {
      "target_name": "soi",
      "sources": [ "soi.cpp" ],
      "libraries": [ "-pthread" ],
      "cxxflags": [ "-g", "-D_FILE_OFFSET_BITS=64", "-MMD", "-MP", "-D_LARGEFILE_SOURCE", "-Wall" ]
    }
  ]
}

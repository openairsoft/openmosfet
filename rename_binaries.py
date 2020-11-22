Import("env")


my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}

progname = "openmosfet.{0}.{1}".format(defines.get("OM_PLATFORM"), defines.get("REPLICA_TYPE"))
env.Replace( PROGNAME=progname )
env["BUILD_FLAGS"].append("-D OM_DEFAULT_BINARY_FILE_NAME='\"{0}.bin\"'".format(progname))

print("Changing bin names")
print("platform : " + defines.get("OM_PLATFORM"))
print("replica type : " + defines.get("REPLICA_TYPE"))
print("progname : " + progname)
print("build flags : ")
print(env["BUILD_FLAGS"])
# config.py

def can_build(platform):
    return True

#TODO: needs to be tested on windows
def configure(env):
    env.Append(LIBS=["opencv_core", "opencv_highgui"]) 
    env.Append(CXXFLAGS=['-mavx'])

    #env.Append(CCFLAGS=["-DLIB_NO_GUI_SUPPORT=1", "-DUSE_AVX_INSTRUCTIONS=1" ])
    #env.ParseConfig('pkg-config opencv --cflags --libs')

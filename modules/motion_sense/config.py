# config.py

def can_build(platform):
    return True

#TODO: needs to be tested on windows
def configure(env):
#env.Append(CXXFLAGS=['-mavx'])
    if env['platform'] == 'windows':
        env.Append(LIBS=["opencv"]) 
    else:
        env.ParseConfig('pkg-config opencv --cflags --libs')
    pass

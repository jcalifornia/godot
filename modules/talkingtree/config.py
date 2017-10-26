# config.py

def can_build(platform):
	return True

def configure(env):
	env.Append(LIBS=['protobuf'])
	pass

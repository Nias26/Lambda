project(
  'Lambda',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
  dependency('ncurses'),
  dependency('threads')
]

source_server = [
  'src/server.c',
]

source_client = [
  'src/client.c',
]

test_sources = [
  'src/main.c'
]

server = executable(
  'server',
  source_server,
  dependencies : dependencies,
  install : true,
)

client = executable(
  'client',
  source_client,
  dependencies : dependencies,
  install : true,
)

tests = executable(
  'tests',
  test_sources,
  dependencies : dependencies,
  install : true,
)

test('basic', server)
test('basic', client)
test('basic', tests)

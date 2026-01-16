project(
  'Lambda',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
  dependency('ncurses')
]

sources = [
  'src/main.c',
]

exe = executable(
  'lambda',
  sources,
  dependencies : dependencies,
  install : true,
)

test('basic', exe)

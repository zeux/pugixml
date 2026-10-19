import pathlib
import subprocess
import sys
import tarfile
import tempfile
import unittest
import zipfile


class ArchiveTest(unittest.TestCase):
    def test_release_file_contents(self):
        root = pathlib.Path(__file__).resolve().parents[1]
        sources = ['src/pugiconfig.hpp'] + sorted(
            path.relative_to(root).as_posix() for path in (root / 'tests/data').iterdir()
        )
        with tempfile.TemporaryDirectory() as directory:
            for extension in ('.tar.gz', '.zip'):
                with self.subTest(extension=extension):
                    target = pathlib.Path(directory) / ('release' + extension)
                    subprocess.check_call([
                        sys.executable, 'scripts/archive.py', str(target), 'release',
                        '1700000000', *sources
                    ], cwd=root)
                    if extension == '.zip':
                        with zipfile.ZipFile(target) as archive:
                            contents = {path: archive.read('release/' + path) for path in sources}
                    else:
                        with tarfile.open(target) as archive:
                            contents = {
                                path: archive.extractfile('release/' + path).read()
                                for path in sources
                            }
                    for path in sources:
                        expected = (root / path).read_bytes()
                        if path.startswith('src/'):
                            expected = expected.replace(b'\r', b'')
                            if extension == '.zip':
                                expected = expected.replace(b'\n', b'\r\n')
                        self.assertEqual(contents[path], expected, path)


if __name__ == '__main__':
    unittest.main()

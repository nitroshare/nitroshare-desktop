"""
Nemo Extension
Adds a "send with NitroShare" menu item to files and directories

The MIT License (MIT)

Copyright (c) 2016 Nathan Osman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

from json import loads
from os import path
from urllib import url2pathname
from urlparse import urlparse

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('Nemo', '3.0')

from gi.repository import Nemo, GObject, Gtk
from requests import ConnectionError, post


class NitroShareMenu(GObject.GObject, Nemo.MenuProvider):
    """
    Add a menu item "Send with NitroShare".
    """

    _LOCALFILE = path.join(path.expanduser('~'), '.NitroShare')

    def send_items(self, menu, window, files, port, token):
        """
        Attempt to send the files using NitroShare.
        """
        try:
            post(
                url='http://127.0.0.1:{}/sendItems'.format(port),
                json={'items': files},
                headers={'X-Auth-Token': token},
            )
        except ConnectionError as e:
            dialog = Gtk.MessageDialog(
                window,
                Gtk.DialogFlags.MODAL,
                Gtk.MessageType.ERROR,
                Gtk.ButtonsType.OK,
                "Unable to communicate with NitroShare.",
            )
            dialog.format_secondary_text(str(e))
            dialog.run()
            dialog.destroy()

    def get_file_items(self, window, files):
        """
        Return a menu item if some files were selected.
        """
        if not len(files):
            return
        item = Nemo.MenuItem(
            name='NitroShare::SendFiles',
            label="Send with NitroShare...",
        )
        try:
            with open(self._LOCALFILE, 'r') as f:
                d = loads(f.read())
            port = d['port']
            token = d['token']
        except (IOError, KeyError):
            item.set_property('sensitive', False)
        else:
            item.connect(
                'activate',
                self.send_items,
                window,
                [url2pathname(urlparse(x.get_uri()).path) for x in files],
                port,
                token,
            )
        return [item]

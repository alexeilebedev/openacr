/*
  The box reads as one line of text whose words are links, and becomes an input the
  moment a reader wants to change it.  Two faces rather than one, because neither
  element can do both: an input holds no anchors, and a line of anchors cannot be
  typed into -- and one element that is contenteditable cannot tell a click meant to
  follow a link from a click meant to place the caret.

  The input is what the page shows on its own, so nothing here is needed for the box
  to work.  What the script adds is the linked line, and the fragment.

  A click places the caret and selects nothing, because a reader who clicks a line is
  reaching into the text they can see -- and a selection would mean their next keystroke
  wiped it.  The `/` and Ctrl-K shortcuts do select, and they select the location rather
  than the whole line: the flags before it are how much of a record the reader asked to be
  shown, which is a standing decision, so starting a new key leaves it in force.

  The text itself is never touched: both faces carry the page's own canonical text, so
  what a reader clicks into is exactly what they were looking at.

  The fragment is the one part of a page's state the server never learns: a browser
  keeps it, so nothing sent to a server can carry it and nothing rendered there can
  show it.  So the script appends it to both faces, and keeps doing so as it changes.
  A reader who followed a table of contents entry then has the whole of where they are
  in the box, and submitting it takes them back.
*/
(function () {
  var find = document.querySelector('.find');
  var shown = find && find.querySelector('.shown');
  var text = find && find.querySelector('.text');
  if (find && shown && text) {
    var base = text.value;
    var basehtml = shown.innerHTML;
    var mark = function () {
      var frag = window.location.hash;
      text.value = base + frag;
      shown.innerHTML = basehtml + (frag ? '<span class="frag"></span>' : '');
      if (frag) {
        shown.querySelector('.frag').textContent = frag;
      }
    };
    var edit = function (pick) {
      var value = text.value;
      var at = value.length;
      var space;
      if (pick) {
        at = 0;
        while (value.charAt(at) === '-') {
          space = value.indexOf(' ', at);
          if (space < 0) {
            break;
          }
          at = space + 1;
        }
      }
      find.classList.remove('js');
      text.focus();
      text.setSelectionRange(at, value.length);
    };
    mark();
    window.addEventListener('hashchange', mark);
    find.classList.add('js');
    /*
      A site is files, and the host serving them answers a path and reads no query -- so
      the form, submitted as a server expects, lands on the index with the key thrown away.
      On a site the box therefore does what a link does: it spells the file the typed
      location names, by the rule every link on the page was written with -- a document's
      .md becomes .html, a directory becomes its index.html, anything else gains .html,
      and a % is escaped -- and goes there.  What it types back is what the box showed, so
      a key copied off one page opens that page.  A key only a server could resolve, such
      as a tool's bare name, names no file, and the host answers with the site's own 404.
    */
    if (find.dataset.site !== undefined) {
      find.addEventListener('submit', function (ev) {
        var root = find.getAttribute('action').replace(/\/$/, '');
        var typed = text.value.replace(/^\s+|\s+$/g, '');
        var hash = typed.indexOf('#');
        var loc = hash >= 0 ? typed.slice(0, hash) : typed;
        var frag = hash >= 0 ? typed.slice(hash) : '';
        var leaf;
        if (/\.md$/.test(loc)) {
          leaf = loc.slice(0, -3) + '.html';
        } else if (loc === '' || /\/$/.test(loc)) {
          leaf = loc + 'index.html';
        } else if (/\.html$/.test(loc)) {
          leaf = loc;
        } else {
          leaf = loc + '.html';
        }
        ev.preventDefault();
        window.location.href = root + '/' + leaf.replace(/%/g, '%25') + frag;
      });
    }
    shown.addEventListener('click', function (ev) {
      if (!ev.target.closest('a')) {
        edit(false);
      }
    });
    text.addEventListener('keydown', function (ev) {
      if (ev.key === 'Escape') {
        mark();
        find.classList.add('js');
      }
    });
    text.addEventListener('blur', function () {
      find.classList.add('js');
    });
    document.addEventListener('keydown', function (ev) {
      var at = document.activeElement;
      var typing = at && (at.tagName === 'INPUT' || at.tagName === 'TEXTAREA' || at.isContentEditable);
      var slash = ev.key === '/' && !typing && !ev.ctrlKey && !ev.metaKey && !ev.altKey;
      var kay = (ev.key === 'k' || ev.key === 'K') && (ev.ctrlKey || ev.metaKey);
      if (slash || kay) {
        ev.preventDefault();
        edit(true);
      }
    });
  }
})();
/*
  The button in the header turns the page over, and the choice is remembered for the next
  page the reader opens.  Three states rather than two: the reader has asked for light, has
  asked for dark, or has asked for nothing and takes what their system says.  Cycling
  through all three is what lets them get back to the system answer once they have left it.
*/
(function () {
  var button = document.querySelector('.theme');
  var order = ['', 'light', 'dark'];
  if (button) {
    button.addEventListener('click', function () {
      var now = document.documentElement.dataset.theme || '';
      var next = order[(order.indexOf(now) + 1) % order.length];
      document.documentElement.dataset.theme = next;
      try { localStorage.setItem('doc-theme', next); } catch (e) {}
    });
  }
})();
/*
  A mermaid block is a picture, and the library that draws it is fetched only by a page
  that carries one -- four pages of the twelve hundred.  A page with none loads nothing,
  which is why this asks before it appends.

  What is in the element until then is the source as it was written, so a browser with no
  network, or one the library fails to load in, shows what the terminal shows.  A reader is
  never left with a blank where a picture should be.
*/
(function () {
  var diagram = document.querySelectorAll('pre.mermaid');
  var tag;
  if (diagram.length) {
    tag = document.createElement('script');
    tag.type = 'module';
    tag.textContent =
      "import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs';\n" +
      "mermaid.initialize({ startOnLoad: true, theme: 'dark' });";
    document.body.appendChild(tag);
  }
})();
/*
  The glyph in a box header copies that box.  A listing is selectable text and always
  was, so nothing here is needed to get the text out of the page -- what this saves is
  the selecting, on a box that is taller than the screen.
*/
(function () {
  var boxes = document.querySelectorAll('.box .copy');
  var i;
  for (i = 0; i < boxes.length; i++) {
    boxes[i].addEventListener('click', function (ev) {
      var box = ev.target.closest('.box');
      var pre = box && box.querySelector('pre');
      var text = pre ? pre.textContent : '';
      var glyph = ev.target.closest('.copy');
      if (text && glyph && navigator.clipboard) {
        navigator.clipboard.writeText(text);
        glyph.classList.add('ok');
        setTimeout(function () { glyph.classList.remove('ok'); }, 900);
      }
    });
  }
})();

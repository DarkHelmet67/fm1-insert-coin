# 13 · Counting visitors without tracking them

How many people open the installer, install the firmware, download the package? Luca asked for it to measure the interest in the project. GitHub Pages keeps no logs of its visitors and counts no downloads, so the site counts them itself, with [GoatCounter](https://www.goatcounter.com): open source, free for non-commercial projects, and made to count visits without following anyone.

## What is counted

| What | Where | Seen on GoatCounter as |
| --- | --- | --- |
| A visit to a page | the front page, the installer, the simulator | the page's path, for example `/fm1-insert-coin/installer/` |
| An install started | the installer, the Install button | event `install-start/insert-coin` |
| An install finished | the installer, when the FM-1 reports the new firmware | event `install-done/insert-coin` |
| A package downloaded | the installer, the link to the `.fwsc` | event `download/insert-coin` |

GoatCounter sets no cookies and keeps no personal data: no IP addresses and no identifiers that follow a visitor from day to day. It records the page, where the visitor came from, the browser and the screen size, and the country. That is why the site needs no consent banner. Each visitor can turn the count off for their own browser by opening any page of the site with `#toggle-goatcounter` at the end of the address.

## Only the published site

The counter lives in one file, `site/analytics.html`, which `tools/sim/bundle.py` and `tools/installer/bundle.py` put in the `<head>` of each page in place of the comment `<!-- analytics -->`. It loads GoatCounter's script only when the page comes from `darkhelmet67.github.io` and is not the preview of a pull request (`/pr-<number>/`). A copy opened from disk or from `localhost`, the tests, the previews and anyone's fork count nothing and load nothing from outside.

The installer's events go through `countEvent()` in `installer/installer.js`, which does nothing when the counter is not there. Only the event's name and the package travel; the package's bytes still go to the synth alone.

## Changing it

To count on another GoatCounter account (a fork, for example), change the two addresses in `site/analytics.html`: the site checked (`darkhelmet67.github.io`) and the account's endpoint (`https://<code>.goatcounter.com/count`). To stop counting, empty that file.

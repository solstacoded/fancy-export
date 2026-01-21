# fancy export
1.9 mod for exporting your level to 2.2 but with extra stuff if you want it

**WARNING:** this mod is not yet thoroughly tested and is in active development.
if in doubt, [use the python script instead](https://github.com/solstacoded/fancy-1.9-level-reuploader)
## current features
- <cy>level processing</c>
	- layer fixing
	- white channel fix
	- various 2.2 bug fixes/unfixes
	- processing options info

- <cy>export options</c>
	- upload to 2.2 servers
		- upload unlisted
		- change level name

## changelog
refer to [changelog.md](changelog.md)

## planned features
- <cy>level processing</c>
    - [ ] glow dot merging
    - [ ] touch trigger hitbox fix
    - [ ] MAYBE: general hitbox fix
    - [x] add actual images for the processing options infoboxes

- <cy>export options</c>
    - [ ] gmd export
    - [ ] LEVEL UPLOAD: change song

## planned technical stuff
you don't really need to pay attention to this section unless you're coding the mod it's just for my reference
 - [x] create textarea that doesn't suck
 - [ ] create an actual level object for processing objects
 - [ ] figure out how to tasks and coroutines
 - [ ] rework processingoptions
## credits
sha-1 implementation - https://github.com/vog/sha1/blob/1.4/sha1.hpp (public domain)
geode popup code - original by geode team, [modified for 1.9 by zmx](https://github.com/qimiko/geode/blob/e13b2fae5fb91be87d15f8ab89f2c8186877f55b/loader/include/Geode/ui/Popup.hpp), further modified by solstacoded
button & square assets - original assets by robtop and geode team, modified by solstacoded
robtop cube asset - by robtop
info assets - put together by solstacoded based upon robtop's level objects
miniz - https://github.com/richgel999/miniz/releases/tag/3.1.0 (public domain)

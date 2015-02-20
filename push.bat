@echo off
git add --all .
set /p comment="Comment for commit : "
git commit -m "%comment%"
git push -u origin master
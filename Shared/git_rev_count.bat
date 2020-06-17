@echo off
echo #pragma once > include/git_version.h

FOR /F "tokens=* USEBACKQ" %%F IN (`git rev-list --count HEAD`) DO (
	SET REV_COUNT=%%F
)

echo #define GIT_REV_COUNT %REV_COUNT% >>  include/git_version.h
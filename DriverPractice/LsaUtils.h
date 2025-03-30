#pragma once
class LsaUtils
{
public:
	static bool ToggleLsa(int PID, UCHAR value);

private:
	static int GetProtectionIndex();
};


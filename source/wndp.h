#pragma once
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
		//èIóπèàóù
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	case WM_SIZE:
	//	G.gSetWindowRect();
		return 0;
	case WM_KEYDOWN:
		switch (wp)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}




LRESULT CALLBACK MyHookProc(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode < 0)    //åàÇ‹ÇËéñ
        return CallNextHookEx(khook, nCode, wp, lp);


    KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lp;

    switch (wp)
    {
    case WM_KEYDOWN:
        if ((p->flags & LLKHF_INJECTED))break;
        if (qwertyMode) {
            if (p->vkCode == 'J' && (GetAsyncKeyState(VK_CONTROL) & 0xf0000000)) {
                qwertyMode = false;
           //     LPushed = false;
                return 0;
            }
            if (p->vkCode!=VK_SPACE && SpaceCnt >= NumOfChangeQwertyBySpaceRenda) {
                qwertyMode = false;
                SpaceCnt = 0;
                break;
            }
        }
        else if (!(GetAsyncKeyState(VK_CONTROL) & 0xf0000000)) {
            if (p->vkCode == VK_SPACE) {
                SpaceCnt++;
                if (SpaceCnt >= NumOfChangeQwertyBySpaceRenda) {
                    qwertyMode = true;
                }
            }
            else {
          
                SpaceCnt = 0;
            }

            if (kmp.KMPs.contains(p->vkCode)) {
                if (kmp.KMPs[p->vkCode].to == 'L'&&!(SpaceCnt >= NumOfChangeQwertyBySpaceRenda)) {
                  
                    qwertyMode = true;
                    INPUT _input = { 0 };
                    _input.type = INPUT_KEYBOARD;
                    _input.ki.wVk = 'L';
                    SendInput(1, &_input, sizeof(INPUT));
                    
                    return TRUE;
                }
                

                INPUT input = { 0 };
                input.type = INPUT_KEYBOARD;

                switch (kmp.KMPs[p->vkCode].cmd)
                {
                case KMP::KMPelm::Cmd::Send:
                    input.ki.wVk = kmp.KMPs[p->vkCode].to;
                    if (1 <= kmp.KMPs[p->vkCode].to && kmp.KMPs[p->vkCode].to <= 254) {
                        SendInput(1, &input, sizeof(INPUT));
                    }
                    else {
                        switch (kmp.KMPs[p->vkCode].to)
                        {
                        case ex_smalltu:
                            /*input.ki.wVk = 'X'; SendInput(1, &input, sizeof(INPUT));
                            input.ki.wVk = 'T'; SendInput(1, &input, sizeof(INPUT));
                            input.ki.wVk = 'U'; SendInput(1, &input, sizeof(INPUT));*/
                            issokuon = true;
                            break;
                        case ex_nn:
                            input.ki.wVk = 'N'; SendInput(1, &input, sizeof(INPUT));
                            input.ki.dwFlags = KEYEVENTF_KEYUP;
                            SendInput(1, &input, sizeof(INPUT));
                            input.ki.dwFlags = 0;
                            input.ki.wVk = 'N'; SendInput(1, &input, sizeof(INPUT));
                            input.ki.dwFlags = KEYEVENTF_KEYUP;
                            SendInput(1, &input, sizeof(INPUT));
                            input.ki.dwFlags = 0;
                            break;
                        case ex_youon:
                            isyouon = true;
                            break;
                        case ex_sakujo:
                            input.ki.wVk = VK_BACK; 
                            SendInput(1, &input, sizeof(INPUT));

                            DSendCnt = 0;
                            issokuon = false;
                            isyouon = false;
                            isShifted = false;

                            break;
                        }
                    }
                    break;
                case KMP::KMPelm::Cmd::DSend:
                    if (DSendCnt == 0) {
                        DSendCnt++;

                        Sendq = std::pair<KMP::Dir, std::vector<KeyCode>>();
                        Sendq = std::pair<KMP::Dir, std::vector<KeyCode>>{ kmp.KMPs[p->vkCode].dir, {kmp.KMPs[p->vkCode].to,kmp.KMPs[p->vkCode].tootherdir} };
                        previousDir = kmp.KMPs[p->vkCode].dir;
                        if ((GetAsyncKeyState(VK_LSHIFT) & 0xf0000000) || (GetAsyncKeyState(VK_RSHIFT) & 0xf0000000)
                            || (GetAsyncKeyState(VK_SHIFT) & 0xf0000000)) {
                            isShifted = true;//TODO:GUIî≈Ç≈Ç»Ç¢ï˚Ç…Ç‡Ç±ÇÍÇà⁄êAÇ∑ÇÈÅBÇ‡Ç§àÍÇ¬Ç†ÇÈÇÃÇ…íçà”
                        }

                    }
                    else {
                        DSendCnt = 0;

                        if (isShifted) {
                            input.ki.wVk = VK_SHIFT;
                            SendInput(1, &input, sizeof(INPUT));
                        }

                        std::pair<KMP::Dir, std::vector<KeyCode>> Send1st = Sendq;
                        std::pair<KMP::Dir, std::vector<KeyCode>> Send2nd = std::pair<KMP::Dir, std::vector<KeyCode>>{ kmp.KMPs[p->vkCode].dir, {kmp.KMPs[p->vkCode].to,kmp.KMPs[p->vkCode].tootherdir} };
                        if (Send1st.second[0] != ex_boin) {
                            if (Send1st.first == KMP::Dir::None || Send2nd.first == KMP::Dir::None) {
                                input.ki.wVk = Send1st.second[0];
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = KEYEVENTF_KEYUP;
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = 0;
                            }
                            else if (Send1st.first == Send2nd.first) {//ë˜âπ
                                input.ki.wVk = Send1st.second[SAMEHAND];
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = KEYEVENTF_KEYUP;
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = 0;
                            }
                            else {//ê¥âπ
                                input.ki.wVk = Send1st.second[ANOTHERHAND];
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = KEYEVENTF_KEYUP;
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = 0;
                            }

                            if (issokuon) {//ë£âπÇ¡
                                issokuon = false;
                                input.ki.dwFlags = KEYEVENTF_KEYUP;
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = 0;
                                SendInput(1, &input, sizeof(INPUT));
                            }

                            if (isyouon) {
                                isyouon = false;

                                input.ki.wVk = 'Y';
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = KEYEVENTF_KEYUP;
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = 0;
                            }

                            if (isShifted) {
                                input.ki.wVk = VK_SHIFT;
                                input.ki.dwFlags = KEYEVENTF_KEYUP;
                                SendInput(1, &input, sizeof(INPUT));
                                input.ki.dwFlags = 0;
                            }
                        }
                        input.ki.wVk = kmp.KMPs[p->vkCode].secboin;
                        SendInput(1, &input, sizeof(INPUT));
                        input.ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(1, &input, sizeof(INPUT));
                        input.ki.dwFlags = 0;

                        isShifted = false;
                    }
                    break;
                }

                return TRUE;
            }
            else {
                return CallNextHookEx(khook, nCode, wp, lp);
            }
        }
        else {
            return CallNextHookEx(khook, nCode, wp, lp);
        }
        break;
    }

    return CallNextHookEx(khook, nCode, wp, lp);
}
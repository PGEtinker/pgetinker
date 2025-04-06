/*
This PGEX was made possible by the following people:
-Testers
My Girlfriend (Thx for letting me borrow your time)
Dragoneye
Javid
KrossX
MaGetzUb
Tutas
Zleapingbear
Huhlig
Bispoo
Lett1

Thx for KrossX for giving me a link to his original work, which helped solve a
few problems
*/
#include "olcPixelGameEngine.h"

#include <string>
#include <utility>
#include <vector>

#pragma region Class Declaration

#ifndef OLC_PGEX_GAMEPAD
#define OLC_PGEX_GAMEPAD

#ifdef WIN32
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#include <oleauto.h>
#include <wbemidl.h>

#if _WIN32_WINNT == _WIN32_WINNT_NT4
#define CURVERSION _WIN32_WINNT_NT4
#elif _WIN32_WINNT == _WIN32_WINNT_WIN2K
#define CURVERSION _WIN32_WINNT_WIN2K
#elif _WIN32_WINNT == _WIN32_WINNT_WINXP
#define CURVERSION _WIN32_WINNT_WINXP
#elif _WIN32_WINNT == _WIN32_WINNT_WS03
#define CURVERSION _WIN32_WINNT_WS03
#elif _WIN32_WINNT == _WIN32_WINNT_WIN6
#define CURVERSION _WIN32_WINNT_WIN6
#elif _WIN32_WINNT == _WIN32_WINNT_WIN7
#define CURVERSION _WIN32_WINNT_WIN7
#elif _WIN32_WINNT == _WIN32_WINNT_WIN8
#define CURVERSION _WIN32_WINNT_WIN8
#elif _WIN32_WINNT == _WIN32_WINNT_WINBLUE
#define CURVERSION _WIN32_WINNT_WINBLUE
#elif _WIN32_WINNT == _WIN32_WINNT_WINXP
#define CURVERSION _WIN32_WINNT_WINXP
#elif _WIN32_WINNT == _WIN32_WINNT_WIN10
#define CURVERSION _WIN32_WINNT_WIN10
#endif

#undef _WIN32_WINNT
#define _WIN32_WINNT 1

#include <Xinput.h>
#undef _WIN32_WINNT
#define _WIN32_WINNT CURVERSION
#pragma comment(lib, "xinput9_1_0.lib")
#endif

#ifdef __linux__

#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <optional>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace olc {
#pragma region Enums
#define GP_BUTTON_COUNT 18
#define GP_AXIS_COUNT 8
enum class GPButtons {
  FACE_D = 1,
  FACE_L = 0,
  FACE_R = 2,
  FACE_U = 3,
  L1 = 4,
  L2 = 6,
  L3 = 10,
  R1 = 5,
  R2 = 7,
  R3 = 11,
  SELECT = 8,
  START = 9,
  DPAD_L = 14,
  DPAD_R = 15,
  DPAD_U = 16,
  DPAD_D = 17
};

enum class GPAxes {
  LX = 1,
  LY = 0,
  RX = 5,
  RY = 2,
  TL = 4,
  TR = 3,
  DX = 6,
  DY = 7,
};
#pragma endregion

class GamePad : public olc::PGEX {
public:
#ifdef WIN32
  GamePad(LPCDIDEVICEINSTANCEA lpddi);
  GamePad(DWORD xId);
#endif
#ifdef __linux__
  GamePad(std::string path, int fd);
#endif
#ifdef __EMSCRIPTEN__
  // TODO ctor
  GamePad(long id, std::string name);
#endif
  GamePad(const GamePad &other) = delete;

  ~GamePad();

  static void init();

  static std::vector<GamePad *> &getGamepads();

  float getAxis(GPAxes a);

  bool hasAxis(GPAxes a);

  HWButton getButton(GPButtons b);

  bool hasButton(GPButtons b);

  std::string getName();

  std::string getId();

  bool stillConnected = true;

  [[nodiscard]] int getAxisCount() const;

  [[nodiscard]] int getButtonCount() const;

  void startVibration(float strength = 1) const;

  void stopVibration() const;

  static GamePad *selectWithButton(olc::GPButtons b);
  static GamePad *selectWithAnyButton();

protected:
  bool OnBeforeUserUpdate(float &fElapsedTime) override;

private:
  GamePad() = default;
  void poll();

  static std::vector<GamePad *> gamepads;

  static void updateGamepads();

  std::string name;
  int axisCount = GP_AXIS_COUNT;
  int buttonCount = GP_BUTTON_COUNT;
  float axes[GP_AXIS_COUNT]{0};
  olc::HWButton buttons[GP_BUTTON_COUNT];
  bool ff = false;
  bool availableButtons[GP_BUTTON_COUNT] = {false};
  bool availableAxes[GP_AXIS_COUNT] = {false};

  void handleButton(int id, bool value);

#ifdef WIN32
  LPDIRECTINPUTDEVICE8A thisDevice;
  std::vector<int> axisPositions;
  LPDIRECTINPUTEFFECT effect = nullptr;
  bool xInput = false;
  DWORD xId;

  static std::thread deviceEnumThread;
  static IDirectInput8A *dev;
#endif
#ifdef __linux__

  int fd = -1;
  ff_effect effect{};
  std::vector<int> maxAbs;
  std::string path;

  void reconnect();
  bool readEvent(input_event &event) const;

  static void enumerateGamepads();
  static GamePad *openGamepad(const std::string &path);
  static X11::Display *display;
  static std::optional<int> inotifyFd;

  constexpr static const int32_t buttonCodes[GP_BUTTON_COUNT]{BTN_X,
                                                              BTN_A,
                                                              BTN_B,
                                                              BTN_Y,
                                                              BTN_TL,
                                                              BTN_TR,
                                                              BTN_TL2,
                                                              BTN_TR2,
                                                              BTN_SELECT,
                                                              BTN_START,
                                                              BTN_THUMBL,
                                                              BTN_THUMBR,
                                                              0,
                                                              0,
                                                              BTN_DPAD_LEFT,
                                                              BTN_DPAD_RIGHT,
                                                              BTN_DPAD_UP,
                                                              BTN_DPAD_DOWN};

  constexpr static const int32_t axisCodes[GP_AXIS_COUNT]{
      ABS_Y, ABS_X, ABS_RY, ABS_RZ, ABS_Z, ABS_RX, ABS_HAT0X, ABS_HAT0Y};
#endif
#ifdef __EMSCRIPTEN__
  long id;

  constexpr static const int32_t buttonCodes[GP_BUTTON_COUNT]{
      2, 0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 14, 15, 12, 13};
#endif
};
} // namespace olc

#endif

#pragma endregion

#ifdef OLC_PGE_GAMEPAD
#undef OLC_PGE_GAMEPAD

#pragma region Custom Dataformat
#ifdef WIN32

struct GamePadState {
  DWORD axes[GP_AXIS_COUNT];
  DWORD povHat;
  BYTE buttons[GP_BUTTON_COUNT];
};

#ifndef DIDFT_OPTIONAL
constexpr DWORD DIDFT_OPTIONAL = 0x80000000;
#endif

DIOBJECTDATAFORMAT g_aObjectFormats[27] = {
    {0, (DWORD)FIELD_OFFSET(GamePadState, axes[1]),
     DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, axes[0]),
     DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, axes[3]),
     DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, axes[2]),
     DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, axes[4]),
     DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, axes[5]),
     DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, povHat),
     DIDFT_POV | DIDFT_ANYINSTANCE, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[0]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[1]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[2]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[3]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[4]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[5]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[6]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[7]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[8]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[9]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[10]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[11]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[12]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[13]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[14]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[15]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[16]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[17]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[18]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
    {0, (DWORD)FIELD_OFFSET(GamePadState, buttons[19]),
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x80000000, 0},
};

DIDATAFORMAT gamepad = {sizeof(DIDATAFORMAT),
                        sizeof(DIOBJECTDATAFORMAT),
                        DIDF_ABSAXIS,
                        sizeof(GamePadState),
                        27,
                        g_aObjectFormats};

#endif
#pragma endregion

#pragma region Class Definition

#pragma region Platform Dependent

#pragma region Windows
#ifdef WIN32

IDirectInput8A *olc::GamePad::dev = nullptr;

std::thread olc::GamePad::deviceEnumThread;

#define SAFE_RELEASE(p)                                                        \
  {                                                                            \
    if ((p)) {                                                                 \
      (p)->Release();                                                          \
      (p) = 0;                                                                 \
    }                                                                          \
  }
BOOL IsXInputDevice(const GUID *pGuidProductFromDirectInput) {
  IWbemLocator *pIWbemLocator = NULL;
  IEnumWbemClassObject *pEnumDevices = NULL;
  IWbemClassObject *pDevices[20] = {0};
  IWbemServices *pIWbemServices = NULL;
  BSTR bstrNamespace = NULL;
  BSTR bstrDeviceID = NULL;
  BSTR bstrClassName = NULL;
  DWORD uReturned = 0;
  bool bIsXinputDevice = false;
  UINT iDevice = 0;
  VARIANT var;
  HRESULT hr;

  // CoInit if needed
  hr = CoInitialize(NULL);
  bool bCleanupCOM = SUCCEEDED(hr);

  // So we can call VariantClear() later, even if we never had a successful
  // IWbemClassObject::Get().
  VariantInit(&var);

  // Create WMI
  hr = CoCreateInstance(__uuidof(WbemLocator), NULL, CLSCTX_INPROC_SERVER,
                        __uuidof(IWbemLocator), (LPVOID *)&pIWbemLocator);
  if (FAILED(hr) || pIWbemLocator == NULL)
    goto LCleanup;

  bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");
  if (bstrNamespace == NULL)
    goto LCleanup;
  bstrClassName = SysAllocString(L"Win32_PNPEntity");
  if (bstrClassName == NULL)
    goto LCleanup;
  bstrDeviceID = SysAllocString(L"DeviceID");
  if (bstrDeviceID == NULL)
    goto LCleanup;

  // Connect to WMI
  hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L, 0L, NULL,
                                    NULL, &pIWbemServices);
  if (FAILED(hr) || pIWbemServices == NULL)
    goto LCleanup;

  // Switch security level to IMPERSONATE.
  CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                    RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
                    EOAC_NONE);

  hr =
      pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
  if (FAILED(hr) || pEnumDevices == NULL)
    goto LCleanup;

  // Loop over all devices
  for (;;) {
    // Get 20 at a time
    hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
    if (FAILED(hr))
      goto LCleanup;
    if (uReturned == 0)
      break;

    for (iDevice = 0; iDevice < uReturned; iDevice++) {
      // For each device, get its device ID
      hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
      if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL) {
        // Check if the device ID contains "IG_".  If it does, then it's
        // an XInput device This information can not be found from
        // DirectInput
        if (wcsstr(var.bstrVal, L"IG_")) {
          // If it does, then get the VID/PID from var.bstrVal
          DWORD dwPid = 0, dwVid = 0;
          WCHAR *strVid = wcsstr(var.bstrVal, L"VID_");
          if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
            dwVid = 0;
          WCHAR *strPid = wcsstr(var.bstrVal, L"PID_");
          if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
            dwPid = 0;

          // Compare the VID/PID to the DInput device
          DWORD dwVidPid = MAKELONG(dwVid, dwPid);
          if (dwVidPid == pGuidProductFromDirectInput->Data1) {
            bIsXinputDevice = true;
            goto LCleanup;
          }
        }
      }
      VariantClear(&var);
      SAFE_RELEASE(pDevices[iDevice]);
    }
  }

LCleanup:
  VariantClear(&var);
  if (bstrNamespace)
    SysFreeString(bstrNamespace);
  if (bstrDeviceID)
    SysFreeString(bstrDeviceID);
  if (bstrClassName)
    SysFreeString(bstrClassName);
  for (iDevice = 0; iDevice < 20; iDevice++)
    SAFE_RELEASE(pDevices[iDevice]);
  SAFE_RELEASE(pEnumDevices);
  SAFE_RELEASE(pIWbemLocator);
  SAFE_RELEASE(pIWbemServices);

  if (bCleanupCOM)
    CoUninitialize();

  return bIsXinputDevice;
}
void olc::GamePad::init() {
  HINSTANCE hinst = GetModuleHandle(NULL);
  DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8A,
                     (LPVOID *)&dev, NULL);

  pge->pgex_Register(new olc::GamePad());

  deviceEnumThread = std::thread{[&]() {
    auto enumFunction = [](LPCDIDEVICEINSTANCEA lpddi, LPVOID pvRef) {
      if (!(lpddi->dwDevType & DI8DEVTYPE_GAMEPAD)) {
        return DIENUM_CONTINUE;
      }

      auto result = reinterpret_cast<std::vector<olc::GamePad *> *>(pvRef);
      if (!IsXInputDevice(&lpddi->guidProduct)) {
        bool found = false;
        for (auto &gamepad : *result) {
          if (gamepad->getId() == lpddi->tszInstanceName) {
            found = true;
            if (!gamepad->stillConnected) {
              gamepad->stillConnected = true;
            }
            break;
          }
        }
        if (found) {
          return DIENUM_CONTINUE;
        }
        result->push_back(new olc::GamePad{lpddi});
      }

      return DIENUM_CONTINUE;
    };
    while (true) {
      for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
        XINPUT_STATE state;
        DWORD dwResult = XInputGetState(i, &state);
        if (dwResult == ERROR_SUCCESS) {
          bool found = false;
          for (auto &gamepad : gamepads) {
            if (gamepad->getId() == std::to_string(i)) {
              found = true;
              if (!gamepad->stillConnected) {
                gamepad->stillConnected = true;
              }
              break;
            }
          }
          if (found) {
            continue;
          }
          gamepads.push_back(new GamePad{i});
        }
      }

      dev->EnumDevices(DI8DEVCLASS_GAMECTRL, enumFunction, (LPVOID)&gamepads,
                       DIEDFL_ALLDEVICES);
    }
  }};
  deviceEnumThread.detach();
}

void olc::GamePad::updateGamepads() {}

inline olc::GamePad::GamePad(LPCDIDEVICEINSTANCEA lpddi) {
  name = lpddi->tszInstanceName;

  dev->CreateDevice(lpddi->guidInstance, &thisDevice, NULL);

  DIDEVCAPS caps;
  caps.dwSize = sizeof(DIDEVCAPS);
  thisDevice->GetCapabilities(&caps);

  DIPROPDWORD bufferSize;
  bufferSize.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  bufferSize.diph.dwSize = sizeof(DIPROPDWORD);
  bufferSize.diph.dwHow = DIPH_DEVICE;
  bufferSize.diph.dwObj = 0;
  bufferSize.dwData = 128;
  thisDevice->SetProperty(DIPROP_BUFFERSIZE, &bufferSize.diph);
  ff = caps.dwFlags & DIDC_FORCEFEEDBACK;

  thisDevice->SetDataFormat(&gamepad);

  auto enumDOFunction = [](LPCDIDEVICEOBJECTINSTANCEA lpddoi, LPVOID pvRef) {
    IDirectInputDevice8A *thisDevice;
    std::vector<int> *axisPositions;

    std::tie(thisDevice, axisPositions) = *reinterpret_cast<
        std::tuple<IDirectInputDevice8A *, std::vector<int> *> *>(pvRef);

    // Axis
    if (lpddoi->dwType & DIDFT_AXIS) {
      DIPROPRANGE range;
      range.diph.dwSize = sizeof(DIPROPRANGE);
      range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
      range.diph.dwHow = DIPH_BYID;
      range.diph.dwObj = lpddoi->dwType;
      range.lMin = -1000;
      range.lMax = 1000;
      thisDevice->SetProperty(DIPROP_RANGE, &range.diph);
      axisPositions->push_back((lpddoi->dwType >> 8) & 0xffff);
    }

    // Button
    if (lpddoi->dwType & DIDFT_BUTTON) {
    }

    // POVhat
    if (lpddoi->dwType & DIDFT_POV) {
    }

    return DIENUM_CONTINUE;
  };

  for (int i = 0; i < GP_BUTTON_COUNT; i++) {
    availableButtons[i] = true;
  }
  for (int i = 0; i < GP_AXIS_COUNT; i++) {
    availableAxes[i] = true;
  }

  auto input = std::make_tuple(thisDevice, &axisPositions);

  thisDevice->EnumObjects(enumDOFunction, &input, DIDFT_ALL);

  int min = 0x7fffffff;
  for (auto p : axisPositions)
    min = std::min(min, p);

  for (auto &p : axisPositions)
    p -= min;

  for (int i = 0; i < GP_AXIS_COUNT; i++)
    axes[i] = 0;
  for (int i = 0; i < GP_BUTTON_COUNT; i++) {
    buttons[i].bHeld = false;
    buttons[i].bPressed = false;
    buttons[i].bReleased = false;
  }

  if (ff) {
    DIPERIODIC periodic;
    periodic.dwMagnitude = DI_FFNOMINALMAX;
    periodic.lOffset = 0;
    periodic.dwPhase = 0;
    periodic.dwPeriod = DWORD(0.05f * DI_SECONDS);
    DIEFFECT eff;
    eff.dwSize = sizeof(DIEFFECT);
    eff.dwDuration = INFINITE;
    eff.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    eff.dwSamplePeriod = 0;
    eff.dwGain = DI_FFNOMINALMAX;
    eff.cAxes = 2;
    DWORD axes[] = {DIJOFS_X, DIJOFS_Y};
    eff.rgdwAxes = axes;
    LONG lDirection[2] = {0, 0};
    eff.rglDirection = lDirection;
    eff.cbTypeSpecificParams = sizeof(periodic);
    eff.lpvTypeSpecificParams = &periodic;

    thisDevice->CreateEffect(GUID_Sine, &eff, &effect, NULL);
    if (effect) {
      effect->Download();
      effect->Stop();
    } else {
      ff = false;
    }
  }

  thisDevice->Acquire();
}

inline olc::GamePad::GamePad(DWORD xId) : xId(xId), xInput(true) {
  XINPUT_CAPABILITIES caps;
  XInputGetCapabilities(xId, 0, &caps);
  for (int i = 0; i < GP_BUTTON_COUNT; i++) {
    availableButtons[i] = true;
  }
  for (int i = 0; i < GP_AXIS_COUNT; i++) {
    availableAxes[i] = true;
  }
  name = "XInput Controller";
}

void olc::GamePad::poll() {
  if (!stillConnected) {
    return;
  }

  for (int i = 0; i < GP_BUTTON_COUNT; i++) {
    buttons[i].bPressed = false;
    buttons[i].bReleased = false;
  }

  if (!xInput) {
    DIDEVICEOBJECTDATA data[128];

    if (FAILED(thisDevice->Poll())) {
      thisDevice->Acquire();
      stillConnected = false;
      return;
    }

    DWORD items = 128;
    thisDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), data, &items, 0);

    for (size_t i = 0; i < items; i++) {
      // Button
      if (data[i].dwOfs >= (DWORD)FIELD_OFFSET(GamePadState, buttons[0]) &&
          data[i].dwOfs <= (DWORD)FIELD_OFFSET(GamePadState, buttons[19])) {
        int id = data[i].dwOfs - FIELD_OFFSET(GamePadState, buttons[0]);
        bool pressed = data[i].dwData & 0x80;
        handleButton(id, pressed);
      }

      // POVhat
      if (data[i].dwOfs == FIELD_OFFSET(GamePadState, povHat)) {
        switch (data[i].dwData / DI_DEGREES / 45) {
        case 0:
          axes[6] = 0;
          axes[7] = -1;
          break;
        case 1:
          axes[6] = 1;
          axes[7] = -1;
          break;
        case 2:
          axes[6] = 1;
          axes[7] = 0;
          break;
        case 3:
          axes[6] = 1;
          axes[7] = 1;
          break;
        case 4:
          axes[6] = 0;
          axes[7] = 1;
          break;
        case 5:
          axes[6] = -1;
          axes[7] = 1;
          break;
        case 6:
          axes[6] = -1;
          axes[7] = 0;
          break;
        case 7:
          axes[6] = -1;
          axes[7] = -1;
          break;
        default:
          axes[6] = 0;
          axes[7] = 0;
        }

        int x = int(axes[6]), y = int(axes[7]);
        handleButton(14, x == -1);
        handleButton(15, x == 1);
        handleButton(16, y == -1);
        handleButton(17, y == 1);
      }

      // Axis
      if (data[i].dwOfs >= (DWORD)FIELD_OFFSET(GamePadState, axes[0]) &&
          data[i].dwOfs <= (DWORD)FIELD_OFFSET(GamePadState, axes[5])) {
        int id = ((data[i].dwOfs - offsetof(GamePadState, axes[0])) / 4);
        int val = data[i].dwData;
        axes[axisPositions[id]] = val / 1000.0f;
      }
    }
  } else {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));
    int res = XInputGetState(xId, &state);

    if (res == ERROR_DEVICE_NOT_CONNECTED) {
      stillConnected = false;
      return;
    }

    axes[2] = -state.Gamepad.sThumbRY / 32768.0f;
    axes[5] = state.Gamepad.sThumbRX / 32768.0f;
    axes[0] = -state.Gamepad.sThumbLY / 32768.0f;
    axes[1] = state.Gamepad.sThumbLX / 32768.0f;

    axes[4] = state.Gamepad.bLeftTrigger / 255.0f;
    axes[3] = state.Gamepad.bRightTrigger / 255.0f;

    bool u, d, l, r;
    u = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
    d = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
    l = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
    r = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;

    axes[6] = float(r - l);
    axes[7] = float(d - u);

    WORD buttonCodes[]{
        XINPUT_GAMEPAD_X,
        XINPUT_GAMEPAD_A,
        XINPUT_GAMEPAD_B,
        XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_LEFT_SHOULDER,
        XINPUT_GAMEPAD_RIGHT_SHOULDER,
        0,
        0,
        XINPUT_GAMEPAD_BACK,
        XINPUT_GAMEPAD_START,
        XINPUT_GAMEPAD_LEFT_THUMB,
        XINPUT_GAMEPAD_RIGHT_THUMB,
        0,
        0,
        XINPUT_GAMEPAD_DPAD_LEFT,
        XINPUT_GAMEPAD_DPAD_RIGHT,
        XINPUT_GAMEPAD_DPAD_UP,
        XINPUT_GAMEPAD_DPAD_DOWN,
    };

    for (size_t i = 0; i < GP_BUTTON_COUNT; i++) {
      bool pressed = state.Gamepad.wButtons & buttonCodes[i];
      handleButton(i, pressed);
    }

    handleButton(6, axes[4] > 0.75f);
    handleButton(7, axes[3] > 0.75f);
  }
}

void olc::GamePad::startVibration(float strength) const {
  if (xInput) {
    if (strength < 0)
      strength = 0;
    if (strength > 1)
      strength = 1;

    XINPUT_VIBRATION vib;
    ZeroMemory(&vib, sizeof(XINPUT_VIBRATION));
    vib.wLeftMotorSpeed = short(65535 * strength);
    vib.wRightMotorSpeed = short(65535 * strength);
    XInputSetState(xId, &vib);
  } else if (ff) {
    DIEFFECT gainMod;
    gainMod.dwGain = DWORD(DI_FFNOMINALMAX * strength);
    effect->SetParameters(&gainMod, DIEP_GAIN);
    effect->Download();
    effect->Start(1, DIES_SOLO);
  }
}

void olc::GamePad::stopVibration() const {
  if (xInput)
    startVibration(0);
  else {
    if (ff)
      effect->Stop();
  }
}

olc::GamePad::~GamePad() {}

std::string olc::GamePad::getId() {
  return xInput ? std::to_string(xId) : name;
}

#endif
#pragma endregion

#pragma region Linux

#ifdef __linux__

X11::Display *olc::GamePad::display{};
std::optional<int> olc::GamePad::inotifyFd = std::nullopt;

olc::GamePad *olc::GamePad::openGamepad(const std::string &path) {
  // Parse the bit array from ioctl
  auto getNthBit = [](const unsigned char *bits, int n) {
    return (bits[n / 8] >> (n % 8)) & 1;
  };

  // Open the file, if can't, it's definitely not a device we can use
  int fd = open(path.c_str(), O_RDWR | O_NONBLOCK);
  if (fd < 0) {
    close(fd);
    return nullptr;
  }

  // Get the key events the device could send
  unsigned char keyBits[KEY_CNT];
  ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits);
  // All gamepads *should* have BTN_GAMEPAD
  if (!getNthBit(keyBits, BTN_GAMEPAD)) {
    close(fd);
    return nullptr;
  }

  return new olc::GamePad{path, fd};
}

// Create a gamepad from a path to the event file
olc::GamePad::GamePad(std::string path, int fd)
    : path{std::move(path)}, availableAxes{false},
      availableButtons{false}, effect{}, fd{fd} {
  // Parse the bit array from ioctl
  auto getNthBit = [](const unsigned char *bits, int n) {
    return (bits[n / 8] >> (n % 8)) & 1;
  };

  // Query the deviceName of the device, if it has one
  char deviceName[256] = "";
  if (ioctl(fd, EVIOCGNAME(256), deviceName) > 0) {
    this->name = deviceName;
  } else {
    this->name = "Undefined";
  }

  // Get the buttons the device reports
  unsigned char keyBits[KEY_CNT];
  ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits);

  // Get the axes the device reports
  unsigned char axisBits[ABS_CNT];
  ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(axisBits)), axisBits);

  for (int i = 0; i < GP_BUTTON_COUNT; i++) {
    availableButtons[i] = getNthBit(keyBits, buttonCodes[i]);
  }

  for (int i = 0; i < GP_AXIS_COUNT; i++) {
    availableAxes[i] = getNthBit(axisBits, axisCodes[i]);
    input_absinfo absinfo{};
    ioctl(fd, EVIOCGABS(axisCodes[i]), &absinfo);
    maxAbs.push_back(absinfo.maximum);
  }

  unsigned char features[1 + FF_MAX / 8] = {0};
  ioctl(fd, EVIOCGBIT(EV_FF, sizeof(features)), features);

  // effect data common to both types
  memset(&effect, 0, sizeof(effect));
  effect.replay.delay = 0;
  effect.replay.length = 0xffff;
  effect.id = -1;
  effect.direction = 0;

  if (ff = getNthBit(features, FF_PERIODIC)) {
    // Support Vibrations based on FF_PERIODIC

    effect.u.periodic.waveform = FF_SINE;
    effect.u.periodic.period = 100;
    effect.u.periodic.magnitude = 0x7fff;
    effect.u.periodic.offset = 0;
    effect.u.periodic.phase = 0;
    effect.u.periodic.envelope.attack_length = 0;
    effect.u.periodic.envelope.attack_level = 0x7fff;
    effect.u.periodic.envelope.fade_length = 0;
    effect.u.periodic.envelope.fade_level = 0x7fff;
    effect.type = FF_PERIODIC;

    if (ioctl(fd, EVIOCSFF, &effect) == -1) {
      perror("Error:");
    }
  } else if (ff = getNthBit(features, FF_RUMBLE)) {
    // Support Vibrations based on FF_RUMBLE

    effect.type = FF_RUMBLE;
    effect.u.rumble.weak_magnitude = 0x7fff;
    ;
    effect.u.rumble.strong_magnitude = 0x7fff;
    ;

    if (ioctl(fd, EVIOCSFF, &effect) == -1) {
      perror("Error:");
    }
  }
}

bool olc::GamePad::readEvent(input_event &event) const {
  return read(fd, &event, sizeof(input_event)) == sizeof(input_event);
}

void olc::GamePad::poll() {
  XResetScreenSaver(display);

  struct stat filestat {};
  fstat(fd, &filestat);
  if (filestat.st_nlink == 0) {
    stillConnected = false;
  }

  if (!stillConnected) {
    return;
  }

  for (auto &button : buttons) {
    button.bPressed = false;
    button.bReleased = false;
  }

  input_event event{};
  while (readEvent(event)) {
    if (event.type == EV_KEY && ((event.code & BTN_GAMEPAD) == BTN_GAMEPAD ||
                                 (event.code & BTN_DPAD_UP) == BTN_DPAD_UP)) {
      for (int i = 0; i < GP_BUTTON_COUNT; i++) {
        if (event.code == buttonCodes[i]) {
          bool pressed = event.value;
          handleButton(i, pressed);

          if (i >= 14) {
            if (i == 14)
              axes[6] = -float(pressed);
            if (i == 15)
              axes[6] = float(pressed);
            if (i == 16)
              axes[7] = -float(pressed);
            if (i == 17)
              axes[7] = float(pressed);
          }
          if (i == 6)
            axes[4] = float(pressed);
          if (i == 7)
            axes[3] = float(pressed);
        }
      }
    }
    if (event.type == EV_ABS) {
      for (int i = 0; i < GP_AXIS_COUNT; i++) {
        if (event.code == axisCodes[i]) {
          float value = float(event.value) / maxAbs[i];
          axes[i] = value;

          if (i == 6) {
            handleButton(14, event.value == -1);
            handleButton(15, event.value == 1);
          }
          if (i == 7) {
            handleButton(17, event.value == 1);
            handleButton(16, event.value == -1);
          }
          if (i == 4)
            handleButton(6, axes[4] >= 0.75f);
          if (i == 3)
            handleButton(7, axes[3] >= 0.75f);
        }
      }
    }
  }
}

void olc::GamePad::updateGamepads() {
  // If for some reason we aren't set up to listen for new gamepads, find all
  // the existing ones and set up for listening next frame
  if (!inotifyFd) {
    enumerateGamepads();
    return;
  }

  alignas(alignof(inotify_event))
      uint8_t buf[sizeof(inotify_event) + NAME_MAX + 1];

  while (read(*inotifyFd, buf, sizeof(buf)) > 0) {
    const inotify_event *event = reinterpret_cast<inotify_event *>(buf);

    if (event->mask & IN_CREATE) {
      std::string path = "/dev/input/" + std::string{event->name};

      GamePad *gp = openGamepad(path);

      if (gp != nullptr) {
        gamepads.push_back(gp);
      }
    }
  }

  // If something's wrong with the inotfiy file descriptor, close it and let
  // enumerateGamepads() try to set things up again next frame
  if (errno != EAGAIN) {
    char *errname = strerror(errno);
    close(*inotifyFd);
    inotifyFd = std::nullopt;
  }
}

void olc::GamePad::enumerateGamepads() {
  DIR *dir = opendir("/dev/input");

  for (dirent *elem = readdir(dir); elem != nullptr; elem = readdir(dir)) {
    std::string path = "/dev/input/" + std::string{elem->d_name};
    bool found = false;
    for (auto &gamepad : gamepads) {
      if (gamepad->path == path) {
        found = true;
        if (gamepad->stillConnected) {
          break;
        }
        gamepad->stillConnected = true;
        gamepad->reconnect();
        break;
      }
    }
    if (found) {
      continue;
    }
    GamePad *gp = openGamepad(path);
    if (gp != nullptr) {
      gamepads.push_back(gp);
    }
  }

  closedir(dir);

  // Now that we've found all the existing gamepads, set up to listen for any
  // new ones that might show up
  if (!inotifyFd) {
    inotifyFd = inotify_init1(IN_NONBLOCK);
    if (*inotifyFd < 0)
      return;

    if (inotify_add_watch(*inotifyFd, "/dev/input", IN_CREATE) < 0) {
      close(*inotifyFd);
      inotifyFd = std::nullopt;
      return;
    }
  }
}

std::string olc::GamePad::getId() { return path; }

void olc::GamePad::startVibration(float strength) const {
  if (!ff)
    return;
  if (strength < 0)
    strength = 0;
  if (strength > 1)
    strength = 1;
  int g = int(0xFFFF * strength);
  input_event gain{};
  gain.type = EV_FF;
  gain.code = FF_GAIN;
  gain.value = g;
  write(fd, &gain, sizeof(gain));

  input_event play{};
  play.type = EV_FF;
  play.code = effect.id;
  play.value = 3;
  write(fd, (const void *)&play, sizeof(play));
}

void olc::GamePad::stopVibration() const {
  if (!ff)
    return;
  input_event stop{};
  stop.type = EV_FF;
  stop.code = effect.id;
  stop.value = 0;
  write(fd, (const void *)&stop, sizeof(stop));
}

olc::GamePad::~GamePad() {
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
  stillConnected = false;
}

void olc::GamePad::reconnect() {
  close(fd);
  fd = open(path.c_str(), O_NONBLOCK | O_RDWR);
  effect.id = -1;
  ioctl(fd, EVIOCSFF, &effect);
}

void olc::GamePad::init() {
  pge->pgex_Register(new GamePad());

  display = X11::XOpenDisplay(nullptr);

  // Go find all the gamepads in the startup phase and set up the listening
  // logic for per-frame updates
  enumerateGamepads();
}

#endif

#ifdef __EMSCRIPTEN__
olc::GamePad::GamePad(long id, std::string name) : name{name}, id{id} {}

void olc::GamePad::poll() {
  EmscriptenGamepadEvent state;

  emscripten_get_gamepad_status(id, &state);

  for (int i = 0; i < GP_BUTTON_COUNT; i++) {
    buttons[i].bPressed = false;
    buttons[i].bReleased = false;

    if (state.digitalButton[buttonCodes[i]] && !buttons[i].bHeld) {
      buttons[i].bHeld = true;
      buttons[i].bPressed = true;
    }
    if (!state.digitalButton[buttonCodes[i]] && buttons[i].bHeld) {
      buttons[i].bHeld = false;
      buttons[i].bReleased = true;
    }

    axes[(int)olc::GPAxes::LX] = state.axis[0];
    axes[(int)olc::GPAxes::LY] = state.axis[1];
    axes[(int)olc::GPAxes::RX] = state.axis[2];
    axes[(int)olc::GPAxes::RY] = state.axis[3];
    axes[(int)olc::GPAxes::TL] = state.analogButton[6];
    axes[(int)olc::GPAxes::TR] = state.analogButton[7];
    axes[(int)olc::GPAxes::DX] =
        (float)buttons[(int)olc::GPButtons::DPAD_R].bHeld -
        buttons[(int)olc::GPButtons::DPAD_L].bHeld;
    axes[(int)olc::GPAxes::DY] =
        (float)buttons[(int)olc::GPButtons::DPAD_D].bHeld -
        buttons[(int)olc::GPButtons::DPAD_U].bHeld;
  }
}

void olc::GamePad::updateGamepads() { emscripten_sample_gamepad_data(); }

std::string olc::GamePad::getId() { return std::to_string(id); }

void olc::GamePad::startVibration(float strength) const {}

void olc::GamePad::stopVibration() const {}

olc::GamePad::~GamePad() {}

void olc::GamePad::init() {
  pge->pgex_Register(new GamePad());

  auto gamepadCallback = [](int eventType,
                            const EmscriptenGamepadEvent *gamepadEvent,
                            void *userData) -> EM_BOOL {
    std::vector<olc::GamePad *> *gamepads =
        (std::vector<olc::GamePad *> *)userData;
    if (eventType == EMSCRIPTEN_EVENT_GAMEPADCONNECTED) {
      if (gamepads->size() <= gamepadEvent->index) {
        gamepads->push_back(new olc::GamePad{gamepadEvent->index,
                                             std::string{gamepadEvent->id}});
      } else {
        (*gamepads)[gamepadEvent->index]->stillConnected = true;
      }
    } else {
      std::cout << "Disconnected " << gamepadEvent->index << "\n";
      (*gamepads)[gamepadEvent->index]->stillConnected = false;
    }
    return true;
  };

  emscripten_set_gamepadconnected_callback(&gamepads, true, gamepadCallback);
  emscripten_set_gamepaddisconnected_callback(&gamepads, true, gamepadCallback);
}
#endif

#pragma endregion

#pragma endregion

#pragma region Common

olc::GamePad *olc::GamePad::selectWithButton(olc::GPButtons b) {
  for (auto &gp : gamepads) {
    if (gp->getButton(b).bPressed) {
      return gp;
    }
  }
  return nullptr;
}

olc::GamePad *olc::GamePad::selectWithAnyButton() {
  for (auto &gp : gamepads) {
    for (int b = 0; b < GP_BUTTON_COUNT; b++) {
      if (gp->getButton(olc::GPButtons(b)).bPressed) {
        return gp;
      }
    }
  }
  return nullptr;
}

#ifndef OLC_GAMEPAD_DEADZONE
#define OLC_GAMEPAD_DEADZONE 0.2f
#endif

#ifndef OLC_GAMEPAD_DEADZONE_OUTER
#define OLC_GAMEPAD_DEADZONE_OUTER 0.2f
#endif

float olc::GamePad::getAxis(olc::GPAxes a) {
  float axis = axes[static_cast<int>(a)];
  if (std::abs(axis) < OLC_GAMEPAD_DEADZONE) {
    axis = 0;
  }
  if (std::abs(axis) > 1 - OLC_GAMEPAD_DEADZONE_OUTER) {
    axis = axis > 0 ? 1.0f : -1.0f;
  }

#ifdef WIN32
  if (!xInput && (a == GPAxes::TL || a == GPAxes::TR))
    return (axis + 1) / 2;
#endif
  return axis;
}

olc::HWButton olc::GamePad::getButton(olc::GPButtons b) {
  return buttons[static_cast<int>(b)];
}

std::string olc::GamePad::getName() { return name; }

int olc::GamePad::getAxisCount() const { return axisCount; }

int olc::GamePad::getButtonCount() const { return buttonCount; }

void olc::GamePad::handleButton(int id, bool value) {
  if (value && !buttons[id].bHeld) {
    buttons[id].bHeld = true;
    buttons[id].bPressed = true;
  }
  if (!value && buttons[id].bHeld) {
    buttons[id].bHeld = false;
    buttons[id].bReleased = true;
  }
#ifdef __linux__
  XResetScreenSaver(display);
#endif
}

bool olc::GamePad::hasAxis(GPAxes a) { return availableAxes[int32_t(a)]; }

bool olc::GamePad::hasButton(GPButtons b) {
  return availableButtons[int32_t(b)];
}

bool olc::GamePad::OnBeforeUserUpdate(float &fElapsedTime) {
  updateGamepads();

  for (auto &gamepad : gamepads) {
    if (gamepad->stillConnected) {
      gamepad->poll();
    }
  }
  return false;
}

std::vector<olc::GamePad *> &olc::GamePad::getGamepads() { return gamepads; }

std::vector<olc::GamePad *> olc::GamePad::gamepads;

#pragma endregion

#endif // OLC_PGE_GAMEPAD

#pragma endregion

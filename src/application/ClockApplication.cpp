#include "ClockApplication.hpp"

ClockApplication::ClockApplication() {
    Service::RegisterService(
        this, "clock_app", { }
    );
}

void ClockApplication::Init() {
    _Clock = S("clock", ClockDriver);
    _Led = S("display", LedMatrixDriver);
    _Scroll = S("scroll", ScrollService);

    Service::BindEvent("clock.time-changed", [=](Service*) {
        Time& t = _Clock->Now();

        if (t.Seconds % 30 == 0) {
            _Scroll->message("%d %d %d", t.Day, t.Month, t.Year);
            return;
        }

        _Led->ClearDisplay();

        _Led->DrawDigitBig(0, t.Hours / 10);
        _Led->DrawDigitBig(4, t.Hours % 10);

        _Led->DrawDigitBig(9, t.Minutes / 10);
        _Led->DrawDigitBig(13, t.Minutes % 10);

        _Led->DrawDigitSmall(18, 3, t.Seconds / 10);
        _Led->DrawDigitSmall(21, 3, t.Seconds % 10);

        _Led->Update();
    });
}

void ClockApplication::Update() {

}
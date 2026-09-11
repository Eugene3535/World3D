#pragma once

class MainLoop
{
public:
    explicit MainLoop(class MainWindow& window) noexcept;
    ~MainLoop();

    void operator()(class Camera& camera) noexcept;

private:
    class MainWindow& m_window;
};
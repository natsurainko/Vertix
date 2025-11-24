using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Silk.NET.Windowing;
using System;
using System.Threading;
using System.Threading.Tasks;
using Vertix.Windowing;

namespace Vertix.Host;

public partial class GameApplication : IHost
{
    public IServiceProvider Services { get; }

    public IHost? Host { get; internal set; }

    public GameWindow Window { get; }

    public GameApplication(IServiceProvider provider)
    {
        Services = provider;

        OnLoading();
        Window = Services.GetRequiredService<GameWindow>();
    }

    public void Dispose()
    {
        Host?.Dispose();
    }

    public Task StartAsync(CancellationToken cancellationToken = default)
    {
        var taskCompletionSource = new TaskCompletionSource();
        var hostAppLifetime = Services.GetRequiredService<IHostApplicationLifetime>();

        try
        {
            Window.CoreWindow.Run();
            taskCompletionSource.SetResult();
            hostAppLifetime.StopApplication();
        }
        catch (Exception ex)
        {
            taskCompletionSource.SetException(ex);
        }

        return taskCompletionSource.Task;
    }

    public Task StopAsync(CancellationToken cancellationToken = default)
    {
        return Task.CompletedTask;
    }

    protected virtual void OnLoading() { }
}
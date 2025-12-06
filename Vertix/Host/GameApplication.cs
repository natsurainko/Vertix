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
    public IServiceProvider Services => Host!.Services;

    public IHost? Host { get; internal set; }

    public Task StartAsync(CancellationToken cancellationToken = default)
    {
        var taskCompletionSource = new TaskCompletionSource();
        var hostAppLifetime = Services.GetRequiredService<IHostApplicationLifetime>();
        var gameWindow = Services.GetRequiredService<GameWindow>();

        try
        {
            gameWindow.CoreWindow.Run();
            gameWindow.Graphics.Dispose();

            taskCompletionSource.SetResult();
            hostAppLifetime.StopApplication();
        }
        catch (Exception ex)
        {
            taskCompletionSource.SetException(ex);
        }

        return taskCompletionSource.Task;
    }

    public Task StopAsync(CancellationToken cancellationToken = default) => Task.CompletedTask;

    public void Dispose()
    {
        Host?.Dispose();
    }
}
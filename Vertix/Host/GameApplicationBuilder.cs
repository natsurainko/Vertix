using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Diagnostics.Metrics;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Silk.NET.Windowing;
using System;
using System.Collections.Generic;
using Vertix.Windowing;

namespace Vertix.Host;

public class GameApplicationBuilder<TGameApplication> : IHostApplicationBuilder
    where TGameApplication : GameApplication
{
    private readonly HostApplicationBuilder _builder;
    private WindowOptions? _windowOptions = null;
    private Type? _windowType = null;

    public IDictionary<object, object> Properties => (_builder as IHostApplicationBuilder).Properties;
    public IConfigurationManager Configuration => _builder.Configuration;
    public IHostEnvironment Environment => _builder.Environment;
    public ILoggingBuilder Logging => _builder.Logging;
    public IMetricsBuilder Metrics => _builder.Metrics;
    public IServiceCollection Services => _builder.Services;

    public GameApplicationBuilder()
    {
        _builder = new HostApplicationBuilder();
        Services.AddSingleton<TGameApplication>();
    }

    public GameApplicationBuilder<TGameApplication> ConfigureWindow<TWindow>(WindowOptions windowOptions)
        where TWindow : GameWindow
    {
        _windowOptions = windowOptions;
        _windowType = typeof(TWindow);

        return this;
    }

    public TGameApplication Build()
    {
        if (_windowOptions == null || _windowType == null)
            throw new InvalidOperationException("The WindowOptions must be configured before building");

        IWindow coreWindow = GameWindow.CreateCore(_windowOptions.Value);
        Services.AddSingleton<IWindow>(coreWindow);
        Services.AddSingleton(typeof(GameWindow), _windowType);

        IHost host = _builder.Build();
        TGameApplication application = host.Services.GetRequiredService<TGameApplication>();
        application.Host = host;

        return application;
    }

    public GameApplicationBuilder<TGameApplication> ConfigureServices(Action<IServiceCollection> action)
    {
        action(Services);
        return this;
    }

    public void ConfigureContainer<TContainerBuilder>(IServiceProviderFactory<TContainerBuilder> factory, Action<TContainerBuilder>? configure = null)
        where TContainerBuilder : notnull => _builder.ConfigureContainer(factory, configure);
}

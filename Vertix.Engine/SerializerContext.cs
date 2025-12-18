using System.Text.Json.Serialization;
using Vertix.Engine.Content;

namespace Vertix.Engine;

[JsonSerializable(typeof(ContentManifest))]
internal partial class SerializerContext : JsonSerializerContext { }

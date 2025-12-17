using System.Text.Json.Serialization;
using Vertix.Content;

namespace Vertix;

[JsonSerializable(typeof(ContentManifest))]
internal partial class SerializerContext : JsonSerializerContext { }

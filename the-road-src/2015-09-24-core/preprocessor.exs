#!/usr/bin/elixir

defmodule Preprocessor do
  defmodule ParseException do
    defexception message: nil, param: nil
  end

  def tokenize(buf) do
    Enum.reverse(
      Enum.reduce(
        String.split(buf, "\n"),
        [],
        fn(buf, acc) ->
          [tokenize2(buf, []) | acc]
        end
      )
    )
  end

  def tokenize2("", acc), do: Enum.reverse(acc)
  def tokenize2(<<byte :: binary-size(1), rest :: binary()>>, acc) when byte in ["\"", "'"] do
    matched? = String.ends_with? rest, byte
    case String.split rest, byte, parts: 2 do
      [string] when matched? -> tokenize2 "", [string | acc]
      [string, rest] -> tokenize2 rest, [byte <> string <> byte | acc]
      [_string] -> raise ParseException , message: "could not find matching quote `#{byte}`"
    end
  end
  def tokenize2(buf, acc) do
    [token | rest] = String.split buf, [" ", "\t"], parts: 2, trim: true
    tokenize2 Enum.join(rest, ""), [map_token(token) | acc]
  end

  defp map_token(<<byte, _ :: binary()>> = token) when byte in ?0..?9, do: to_number(token)
  defp map_token(token), do: token

  defp to_number(buf) do
    case Integer.parse buf do
      {int, ""} ->
        int

      {_, "." <> _} ->
        case Float.parse buf do
          {float, ""} ->
            float

          {float, ".." <> rest} ->
            Range.new float, to_number rest

          {_, _} ->
            raise ParseException, message: "error parsing '#{buf}' as float"
        end

      {_, _} ->
        raise ParseException, message: "error parsing '#{buf}' as integer"
    end
  end

  defp macro_type(["type", type, "::", super | args]), do: nil
  defp macro_type(["type", type, "::", super | args]), do: nil
end

input = Path.join([__DIR__, "./input.code"])

tokens = File.read!(input) |> Preprocessor.tokenize


#tokens = buf
#  |> String.split(["\r", "\n"])
#  |> Enum.flat_map(fn(line) ->
#    (line |> String.split(~r/([ \t]+)/)) ++ ["\n"]
#  end)
#  |> Enum.join(" ")
#
#minimalblock = %{
#  ref: :binary.encode_unsigned(:random.uniform(4294967295)) |> Base.encode64,
#  arguments: [],
#  body: [],
#  export: %{
#    types: %{},
#    funs: %{},
#    recvs: %{}
#  }
#}
#
#fmtBlock = fn(%{
#  ref: ref,
#  arguments: args,
#  body: body,
#  export: %{
#    types: types,
#    funs: funs,
#    recvs: recvs
#  }
#}) ->
#  fmtArgs = fn
#    ([]) -> "()"
#    (args) -> "( " <> Enum.join(args, ", ") <> " )"
#  end
#
#  fmtReturn = fn
#    ([]) -> "Nothing"
#    (body) -> List.last(body)
#  end
#
#  """
#  $#{ref}# :: #{fmtReturn.(body)} #{fmtArgs.(args)} {
#  }
#  """
#end
#
#IO.write fmtBlock.(minimalblock)
